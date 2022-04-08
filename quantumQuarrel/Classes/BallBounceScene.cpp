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

USING_NS_CC;

Scene* BallBounce::createScene()
{
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

	playerOne->init(gravity, _MainMap, this->getBoundingBox(), 1);
	playerTwo->init(gravity, _MainMap, this->getBoundingBox(), 2);
	

	playerTwo->playerSprite = Sprite::create("ball_blue.png");
	

	this->addChild(playerOne->playerSprite);
	this->addChild(playerTwo->playerSprite);

	this->scheduleUpdate();
	this->addChild(node);

	return true;
}

void BallBounce::update(float dt) {
	CCLOG("Player Position %f", playerOne->position.y);
	CCLOG("Enemy Position %f", playerTwo->position.y);

	playerOne->update(dt);
	playerTwo->update(dt);
	if (playerOne->Attacked()) {
		if (playerOne->orientation == 1) {
			playerOne->setHitBox(Rect(playerOne->position.x + 10, playerOne->position.y, 10, 10));
		}
		else {
			playerOne->setHitBox(Rect(playerOne->position.x - 10, playerOne->position.y, 10, 10));
		}
		if (checkForCollision(playerOne->hitBox, playerTwo->boundingBox)) {
			playerTwo->boundingBox;
			playerOne->hitBox;
			//Play knockback animation
			calculateKnockback(playerTwo, playerOne->orientation);
		}
	}
	else if (playerTwo->Attacked()) {
		if (playerTwo->orientation == 1) {
			playerTwo->setHitBox(Rect(playerTwo->position.x + 4, playerTwo->position.y, 10, 10));
		}
		else {
			playerTwo->setHitBox(Rect(playerTwo->position.x - 4, playerTwo->position.y, 10, 10));
		}
		if (checkForCollision(playerTwo->hitBox, playerOne->boundingBox)) {
			//Play knockback animation
			calculateKnockback(playerOne, playerTwo->orientation);
		}
	}
	// For debugging purposes
	if (debugMode) {
		node->clear();
		//players
		drawBox(node, Vec2(playerOne->position.x - playerOne->radius, playerOne->position.y - playerOne->radius), Vec2(playerOne->position.x + playerOne->radius, playerOne->position.y + playerOne->radius));
		drawBox(node, Vec2(playerTwo->position.x - playerTwo->radius, playerTwo->position.y - playerTwo->radius), Vec2(playerTwo->position.x + playerTwo->radius, playerTwo->position.y + playerTwo->radius));
		//deathplane
		auto winWidth = _MainMap->getMapSize().width;
		auto winHeight = _MainMap->getMapSize().height;
		for (int tileY = 0; tileY < winHeight; tileY++) {
			for (int tileX = 0; tileX < winWidth; tileX++) {
				auto xPos = tileX * 16;//_MainMap->getTileSize().width;
				auto yPos = tileY * 16;//_MainMap->getTileSize().height;
				node->drawPoint(Vec2(xPos, yPos), 3, Color4F::RED);
				drawBox(node, Vec2(xPos, yPos), Vec2(xPos + _MainMap->getTileSize().width, yPos + _MainMap->getTileSize().height));
			}
		}
	}
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
	/*if (abs(Attacker->getPosition().x - Reciver->getPosition().x) < 10) {// Change this value to be the radius of the sprite
		if (abs(Attacker->getPosition().y - Reciver->getPosition().y) < 10) {
			
		//Insert momentum calculations here
		}
	}*/
}
void BallBounce::calculateKnockback(Player* Reciver, int AttackOrientation) {
	Reciver->acceleration.x+= 20 * AttackOrientation;
	Reciver->acceleration.y+= 50 * AttackOrientation;
}

void BallBounce::drawBox(DrawNode* node, Vec2 bottomLeft, Vec2 topRight) {
	float height = topRight.y - bottomLeft.y;
	node->drawLine(Vec2(bottomLeft.x, bottomLeft.y), Vec2(topRight.x, topRight.y), Color4F::RED);
	node->drawLine(Vec2(bottomLeft.x, bottomLeft.y + height), Vec2(topRight.x, topRight.y - height), Color4F::RED);
	node->drawRect(Vec2(bottomLeft.x, bottomLeft.y), Vec2(topRight.x, topRight.y), Color4F::RED);
}
