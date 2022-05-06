/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "BallBounceScene.h"
#pragma once	
#include "fmod.hpp"	
#include "MainMenu.h"	
#include "Player.h"
#include "Item.h"

USING_NS_CC;
float overall_vol;

Scene* BallBounce::createScene(float volume)
{
	overall_vol = volume;
	auto scene = Scene::create();
	auto layer = BallBounce::create();
	scene->addChild(layer);
	return scene;
}

bool BallBounce::init()
{
	if (!Scene::init())
	{
		return false;
	}
	
	//debugMode = true; //Comment out to remove debugLines

	winSize = cocos2d::Director::getInstance()->getWinSize();
	node = DrawNode::create(2);


	gravity = 400;
	dtF = 0;
	gameTime = 300;

	_MainMap = TMXTiledMap::create("mainMap.tmx");
	auto background = _MainMap->getLayer("Background");
	_ground = _MainMap->getLayer("Collision");
	_ground->setVisible(false);
	_DeathPlane = _MainMap->getLayer("Death_Plane");
	_DeathPlane->setVisible(false);
	_MainMap->setScaleX((winSize.width / _MainMap->getContentSize().width) * 1);
	_MainMap->setScaleY((winSize.height / _MainMap->getContentSize().height) * 1);
	this->addChild(_MainMap);

	

	playerOne = new Player();
	playerTwo = new Player();
	item = new Item();
	itemSpawnCooldown = 35.0;

	playerOne->init(gravity, _MainMap, this->getBoundingBox(), 1, overall_vol);
	playerTwo->init(gravity, _MainMap, this->getBoundingBox(), 2, overall_vol);
	item->init(gravity, RandomHelper::random_int(0, 1), _MainMap);


	this->addChild(playerOne->playerSprite);
	this->addChild(playerTwo->playerSprite);
	this->addChild(item->itemSprite);
	
	this->addChild(playerOne->playerDamageIcon3);
	this->addChild(playerOne->playerDamageIcon2);
	this->addChild(playerOne->playerDamageIcon1);
	this->addChild(playerOne->itemShieldIcon);
	this->addChild(playerOne->itemSwordIcon);
	this->addChild(playerOne->damageLabel);
	
	
	this->addChild(playerTwo->playerDamageIcon3);
	this->addChild(playerTwo->playerDamageIcon2);
	this->addChild(playerTwo->playerDamageIcon1);
	this->addChild(playerTwo->itemShieldIcon);
	this->addChild(playerTwo->itemSwordIcon);
	this->addChild(playerTwo->damageLabel);

	//Creating the timer
	gameTimeMins = gameTime / 60;
	gameTimeSecs = gameTime - gameTimeMins * 60;
	timerGUI = Label::createWithSystemFont(std::to_string(gameTimeMins) + ":" + std::to_string(gameTimeSecs)+ "0", "Arial", 20);
	timerGUI->setAnchorPoint(cocos2d::Vec2(0, 0));
	timerGUI->setColor(Color3B::BLACK);
	auto gui = _MainMap->getObjectGroup("GUI");
	ValueMap timerPos = gui->getObject("Timer");
	timerGUI->setPosition(Vec2(timerPos.at("x").asInt() * _MainMap->getScaleX(), timerPos.at("y").asInt() * _MainMap->getScaleY()));
	this->addChild(timerGUI);

	
	this->scheduleUpdate();
	schedule(CC_SCHEDULE_SELECTOR(BallBounce::timer), 1.0f, kRepeatForever, 0); // get controller input every 0.1 seconds
	this->addChild(node);

	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, nullptr);
	sound_vol.setVolume(overall_vol);

	return true;
}


void BallBounce::update(float dt) {
	//CCLOG("Player Position %f", playerOne->position.y);
	//CCLOG("Enemy Position %f", playerTwo->position.y);
	//CCLOG("Initial Time %f", dtI);
	//CCLOG("Final Time %f", dtF);
	dtI = dtF;
	dtF+= dt;
	playerOne->update(dt);
	playerTwo->update(dt);
	if (item != nullptr) {
		item->update(dt);
	}
	playerWon();
	if (itemSpawnCooldown <= 0) {
		itemSpawnCooldown = 35.0;
		item = new Item();
		item->init(gravity, RandomHelper::random_int(0, 1), _MainMap);
		this->addChild(item->itemSprite);
	}
	else {
		itemSpawnCooldown -= dt;
	}


	if (item != nullptr) {
		if (playerOne->isPickingUp) {
			if (!item->isEquiped) {
				if (checkForCollision(playerOne->attackBox, item->hitBox)) {
					item->pickUp();
					playerOne->hasItem = true;
					playerOne->setItem(item);
				}
			}
		}
		if (playerTwo->isPickingUp) {
			if (!item->isEquiped) {
				if (checkForCollision(playerTwo->attackBox, item->hitBox)) {
					item->pickUp();
					playerTwo->hasItem = true;
					playerTwo->setItem(item);
				}
			}
		}
	}

	if (playerOne->beginComboChain) {
		if (!playerOne->ComboChain(dtF, dtI)) {
			playerOne->beginComboChain = false;
			calculateKnockback(playerTwo, playerOne);
			//Other player is knocked back once the chain ends.
			playerTwo->isStuned = false;
			playerOne->onCooldown = true;
		}
	}
	if (playerOne->Attacked()) {
		if (checkForCollision(playerOne->attackBox, playerTwo->hitBox)) {
			//Play knockback animation
			if (playerOne->onGround) {
				if (!playerOne->onCooldown) {
					if (playerOne->beginComboChain == false) {
						playerOne->beginComboChain = true;
						playerOne->comboStartTime = dtF;
					}
					//Begining a combo chain on the ground
					if (playerOne->ComboChain(dtF, dtI)) {
						dealDamage(playerTwo, playerOne, 2.5);
						playerTwo->isStuned = true;
						//stop old attack sounds	
						attackHitChannel->stop();
						//file path	
						std::string attackHitPath = FileUtils::getInstance()->fullPathForFilename("audio/attack_hit.wav");
						//create sound	
						system->createSound(attackHitPath.c_str(), FMOD_LOOP_OFF, 0, &attackHitSound);
						system->playSound(attackHitSound, 0, true, &attackHitChannel);
						//set volume	
						attackHitChannel->setVolume(sound_vol.getVolume());
						//play
						attackHitChannel->setPaused(false);
					}
				}
			}
			else {
				dealDamage(playerTwo, playerOne, 1.2);
				calculateKnockback(playerTwo, playerOne);

				//stop old attack sounds	
				attackHitChannel->stop();
				//file path	
				std::string attackHitPath = FileUtils::getInstance()->fullPathForFilename("audio/attack_hit.wav");
				//create sound	
				system->createSound(attackHitPath.c_str(), FMOD_LOOP_OFF, 0, &attackHitSound);
				system->playSound(attackHitSound, 0, true, &attackHitChannel);
				//set volume	
				attackHitChannel->setVolume(sound_vol.getVolume());
				//play
				attackHitChannel->setPaused(false);
			}
		}
		else {
			//stop old attack sounds	
			attackMissChannel->stop();
			//file path	
			std::string attackMissPath = FileUtils::getInstance()->fullPathForFilename("audio/attack_miss.wav");
			//create sound	
			system->createSound(attackMissPath.c_str(), FMOD_LOOP_OFF, 0, &attackMissSound);
			system->playSound(attackMissSound, 0, true, &attackMissChannel);
			//set volume	
			attackMissChannel->setVolume(sound_vol.getVolume());
			//play
			attackMissChannel->setPaused(false);
		}
	}
	if (playerTwo->beginComboChain) {
		if (!playerTwo->ComboChain(dtF, dtI)) {
			playerTwo->beginComboChain = false;
			calculateKnockback(playerOne, playerTwo);
			playerOne->isStuned = false;
			playerTwo->onCooldown = true;
		}
		
	}
	if (playerTwo->Attacked()) {
		if (checkForCollision(playerTwo->attackBox, playerOne->hitBox)) {
			if (playerTwo->onGround) {
				if (!playerTwo->onCooldown) {
					if (playerTwo->beginComboChain == false) {
						playerTwo->beginComboChain = true;
						playerTwo->comboStartTime = dtF;
					}
					//Begining a combo chain on the ground
					if (playerTwo->ComboChain(dtF, dtI)) {
						dealDamage(playerOne, playerTwo, 2.5);
						playerOne->isStuned = true;
						//stop old attack sounds	
						attackHitChannel->stop();
						//file path	
						std::string attackHitPath = FileUtils::getInstance()->fullPathForFilename("audio/attack_hit.wav");
						//create sound	
						system->createSound(attackHitPath.c_str(), FMOD_LOOP_OFF, 0, &attackHitSound);
						system->playSound(attackHitSound, 0, true, &attackHitChannel);
						//set volume	
						attackHitChannel->setVolume(sound_vol.getVolume());
						//play
						attackHitChannel->setPaused(false);
					}
				}
			}
			else {
				dealDamage(playerOne, playerTwo, 1.2);
				calculateKnockback(playerOne, playerTwo);
				//stop old attack sounds	
				attackHitChannel->stop();
				//file path	
				std::string attackHitPath = FileUtils::getInstance()->fullPathForFilename("audio/attack_hit.wav");
				//create sound	
				system->createSound(attackHitPath.c_str(), FMOD_LOOP_OFF, 0, &attackHitSound);
				system->playSound(attackHitSound, 0, true, &attackHitChannel);
				//set volume	
				attackHitChannel->setVolume(sound_vol.getVolume());
				//play
				attackHitChannel->setPaused(false);
			}
		}
		else {
			//stop old attack sounds	
			attackMissChannel->stop();
			//file path	
			std::string attackMissPath = FileUtils::getInstance()->fullPathForFilename("audio/attack_miss.wav");
			//create sound	
			system->createSound(attackMissPath.c_str(), FMOD_LOOP_OFF, 0, &attackMissSound);
			system->playSound(attackMissSound, 0, true, &attackMissChannel);
			//set volume	
			attackMissChannel->setVolume(sound_vol.getVolume());
			//play
			attackMissChannel->setPaused(false);
		}
	}
	if (item != nullptr) {
		if (item->setToDespawn) {
			item->itemSprite->removeFromParent();
			if (playerOne->hasItem) {
				playerOne->item = nullptr;
				playerOne->hasItem = false;
			}
			if (playerTwo->hasItem) {
				playerTwo->item = nullptr;
				playerTwo->hasItem = false;
			}
			item = nullptr;
		}
	}

	// For debugging purposes
	if (debugMode) {
		node->clear();
		//players	
		//hitboxes	
		drawBox(node, playerOne->hitBox, Color4F::WHITE);
		drawBox(node, playerTwo->hitBox, Color4F::BLUE);
		//attackboxes	
		drawBox(node, playerOne->attackBox, Color4F::WHITE);
		drawBox(node, playerTwo->attackBox, Color4F::BLUE);
		node->drawPoint(playerOne->footPos, 5, Color4F::WHITE);
		node->drawPoint(playerTwo->footPos, 5, Color4F::BLUE);
		//items
		if (item != nullptr) {
			drawBox(node, item->hitBox, Color4F::GREEN);
			node->drawPoint(item->basePos, 5, Color4F::GREEN);
		}
		//tiles	
		/*
		auto winWidth = _MainMap->getMapSize().width;
		auto winHeight = _MainMap->getMapSize().height;
		for (int tileY = 0; tileY < winHeight; tileY++) {
			for (int tileX = 0; tileX < winWidth; tileX++) {
				auto xPos = tileX * _MainMap->getTileSize().width;
				auto yPos = tileY * _MainMap->getTileSize().height;
				drawBox(node, Vec2(xPos, yPos), Vec2(xPos + _MainMap->getTileSize().width, yPos + _MainMap->getTileSize().height));
			}
		}
		*/
	}
}

void BallBounce::playerWon() {
	if (playerOne->playerLives <= 0) {
		toPostGameScene(2); // player 2 wins
	}
	else if (playerTwo->playerLives <= 0) {
		toPostGameScene(1); // Player 1 wins
	}
}
//Unloads the game and goes to the post game scene
void BallBounce::toPostGameScene(int playerWon) {

	auto postGame = PostGameScene::createScene(playerWon, overall_vol);
	Director::getInstance()->replaceScene(postGame);
}
//Goes back to the main menu
void BallBounce::toMainMenu() {
		auto mainMenu = MainMenu::createScene(true);
		Director::getInstance()->replaceScene(mainMenu);
}
//Countdown timer that will determine how long is left in a game (the function is called every second)
void BallBounce::timer(float dt) {
	//If the players run out of time, then the winner is decided by the one with the most amount of lives
	if (gameTime <= 0) {
		if (playerOne->playerLives > playerTwo->playerLives) {
			toPostGameScene(1);
		}
		else {
			toPostGameScene(2);
		}
	}
	//Converting seconds to minutes and seconds
	gameTimeMins = gameTime / 60;
	gameTimeSecs = gameTime - gameTimeMins * 60;
	
	//Need to add an additional zero when the number of seconds is 0 for formatting.
	if (gameTimeSecs == 0) {
		timerGUI->setString(std::to_string(gameTimeMins) + ":" + std::to_string(gameTimeSecs)+ "0");
	}
	else {
		timerGUI->setString(std::to_string(gameTimeMins) + ":" + std::to_string(gameTimeSecs));
	}
	gameTime -= 1.0;

}

void BallBounce::setViewPointCenter(Vec2 Position) {
	Size winSize = cocos2d::Director::getInstance()->getWinSize(); // Getting the window size
	//These functions Make sure the camera stops following the player once they get "Off screen"
	int x = MAX(Position.x, winSize.width / 2);
	int y = MAX(Position.y, winSize.height / 2);
	x = MIN(x, (_MainMap->getMapSize().width * this->_MainMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (_MainMap->getMapSize().height * _MainMap->getTileSize().height) - winSize.height / 2);

	Vec2 actualposition = Vec2(x, y);
	Vec2 centerOfView = Vec2(winSize.width / 2, winSize.height / 2);
	Vec2 viewPoint = centerOfView;
	viewPoint.subtract(actualposition);

	this->setPosition(viewPoint);
}

bool BallBounce::checkForCollision(Rect Attacker, Rect Reciver) {
	if (Attacker.intersectsRect(Reciver)) {
		CCLOG("Collison");
		return true;
	}
	return false;
}
//Calculates the knockback after the user recives a hit
void BallBounce::calculateKnockback(Player* Reciver, Player* Attacker) {
	Reciver->isStuned = false;
	Reciver->acceleration.x +=  Attacker->orientation *(75+ Reciver->damage);
	if (Reciver->position.y >= Attacker->position.y) {
		Reciver->acceleration.y += 80;
	}
	else {
		Reciver->acceleration.y -= 50;
	}
	Reciver->damageLabel->setString(std::to_string(Reciver->damage).substr(0,4)+ "%");
}

//Calculates the damage after the user recives a hit
void BallBounce::dealDamage(Player* Reciver, Player* Attacker, float dmg) {
	if (Attacker->hasItem) {
		if (Attacker->item->type == 1) {
			dmg *= 1.5;
		}
	}
	if (Reciver->hasItem) {
		if (Reciver->item->type == 0) {
			dmg *= 0.5;
		}
	}
	Reciver->damage += dmg;
}

void BallBounce::drawBox(DrawNode* node, Vec2 bottomLeft, Vec2 topRight) {
	float height = topRight.y - bottomLeft.y;
	node->drawLine(Vec2(bottomLeft.x, bottomLeft.y), Vec2(topRight.x, topRight.y), Color4F::RED);
	node->drawLine(Vec2(bottomLeft.x, bottomLeft.y + height), Vec2(topRight.x, topRight.y - height), Color4F::RED);
	node->drawRect(Vec2(bottomLeft.x, bottomLeft.y), Vec2(topRight.x, topRight.y), Color4F::RED);
}
void BallBounce::drawBox(DrawNode* node, Rect rectangle, Color4F color) {
	auto bottomLeft = Vec2(rectangle.getMinX(), rectangle.getMinY());
	auto topRight = Vec2(rectangle.getMaxX(), rectangle.getMaxY());
	float height = topRight.y - bottomLeft.y;
	node->drawLine(Vec2(bottomLeft.x, bottomLeft.y), Vec2(topRight.x, topRight.y), color);
	node->drawLine(Vec2(bottomLeft.x, bottomLeft.y + height), Vec2(topRight.x, topRight.y - height), color);
	node->drawRect(Vec2(bottomLeft.x, bottomLeft.y), Vec2(topRight.x, topRight.y), color);
}
