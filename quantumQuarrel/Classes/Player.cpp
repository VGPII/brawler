#include "Player.h"
#include "AudioManager.h"	
#include "fmod.hpp"

USING_NS_CC;

#define LS_HORI 0
#define LS_VERT 1
#define RS_HORI 2
#define RS_VERT 3
#define LT 4
#define RT 5

#define A 0
#define B 1
#define X 2
#define Y 3
#define LB 4
#define RB 5
#define SELECT 6
#define START 7
#define LS 8
#define RS 9
#define UP 10
#define RT 11
#define DN 12
#define LF 13
#define JUMP_TAG 3
#define ATTACK_TAG 2
#define WALK_TAG 1
#define IDLE_TAG 0
#define NULL_TAG -1
#define SPRITE_SCALE 0.4	

bool Player::init(int gravStrength, TMXTiledMap* initMap, Rect initBoundingBox, int playerNumberInit)
{
	boundingBox = initBoundingBox;
	_CurMap = initMap;
	_background = _CurMap->getLayer("Background");
	_ground = _CurMap->getLayer("Collision");
	_DeathPlane = _CurMap->getLayer("Death_Plane");
	//Setting number of animation frames
	NUM_IDLE_FRAMES = 1;
	NUM_JUMP_FRAMES = 6;
	NUM_WALK_FRAMES = 5;
	NUM_ATTACK_FRAMES = 5;

	playerLives = 3;
	damage = 0.0;
	comboCooldownTime = 0.5;
	comboCooldown = comboCooldownTime;
	onCooldown = false;
	
	objectGroup = _CurMap->getObjectGroup("SpawnPoints");
	playerNumber = playerNumberInit;
	guiGroup = _CurMap->getObjectGroup("GUI");

	item = nullptr;

	if (objectGroup == NULL) {
		CCLOG("Tile map does not have an object layer");
	}
	
	
	
	if (playerNumber == 1) {
		playerSprite = Sprite::create("/PlayerAnimation/player1/hit_reaction/react_1.png");
		playerSprite->setScale(SPRITE_SCALE);
		ValueMap spawnPoint = objectGroup->getObject("SpawnPointP1");
		Spawnpoint = Vec2(spawnPoint.at("x").asInt() * _CurMap->getScaleX(), spawnPoint.at("y").asInt()* _CurMap->getScaleY());
		orientation = 1; // Facing towards the right side of the screen
		
		ValueMap player1Pos = guiGroup->getObject("Player1Damage");
		//Setting player 1 GUI elements
		playerDamageIcon3 = Sprite::create("/Player1DamageIcons/3_life.png");
		playerDamageIcon2 = Sprite::create("/Player1DamageIcons/2_life.png");
		playerDamageIcon2->setVisible(false);
		playerDamageIcon1 = Sprite::create("/Player1DamageIcons/1_life.png");
		playerDamageIcon1->setVisible(false);
		playerDamageIcon3->setPosition(Vec2(player1Pos.at("x").asInt() * _CurMap->getScaleX(), player1Pos.at("y").asInt() * _CurMap->getScaleY()));
		playerDamageIcon2->setPosition(playerDamageIcon3->getPosition());
		playerDamageIcon1->setPosition(playerDamageIcon3->getPosition());
		playerDamageIcon3->setVisible(true);
		playerDamageIcon3->setScale(0.5);
		playerDamageIcon2->setScale(0.5);
		playerDamageIcon1->setScale(0.5);
		playerSprite->setColor(Color3B::RED);
		damageLabel = Label::createWithSystemFont("0.0%", "Arial", 20);
		damageLabel->setAnchorPoint(cocos2d::Vec2(0, 0));
		damageLabel->setPosition(playerDamageIcon1->getPosition().x+40, playerDamageIcon1->getPosition().y+25);
		
		loadAnimations();
	}
	else if (playerNumber == 2) {
		playerSprite = Sprite::create("/PlayerAnimation/player2/hit_reaction/react_1.png");
		playerSprite->setScale(SPRITE_SCALE);
		ValueMap spawnPoint = objectGroup->getObject("SpawnPointP2");
		Spawnpoint = Vec2(spawnPoint.at("x").asInt() * _CurMap->getScaleX(), spawnPoint.at("y").asInt() * _CurMap->getScaleY());
		orientation = -1; // Facing towards the left side of the screen
		playerSprite->setColor(Color3B::BLUE);
		playerSprite->setScaleX(-1 * SPRITE_SCALE);
		ValueMap player2Pos = guiGroup->getObject("Player2Damage");
		playerDamageIcon3 = Sprite::create("/Player2DamageIcons/3_life.png");
		playerDamageIcon2 = Sprite::create("/Player2DamageIcons/2_life.png");
		playerDamageIcon2->setVisible(false);
		playerDamageIcon1 = Sprite::create("/Player2DamageIcons/1_life.png");
		playerDamageIcon1->setVisible(false);
		playerDamageIcon3->setPosition(Vec2(player2Pos.at("x").asInt() * _CurMap->getScaleX(), player2Pos.at("y").asInt() * _CurMap->getScaleY()));
		playerDamageIcon2->setPosition(playerDamageIcon3->getPosition());
		playerDamageIcon2->setScale(0.5);
		playerDamageIcon1->setPosition(playerDamageIcon3->getPosition());
		playerDamageIcon1->setScale(0.5);
		playerDamageIcon3->setVisible(true);
		playerDamageIcon3->setScale(0.5);
		damageLabel = Label::createWithSystemFont("0.0%", "Arial", 20);
		damageLabel->setAnchorPoint(cocos2d::Vec2(0, 0));
		damageLabel->setPosition(playerDamageIcon1->getPosition().x + 40, playerDamageIcon1->getPosition().y + 25);
		loadAnimations();
	}
	
	position = Spawnpoint;
	velocity = cocos2d::Vec2(0, 0);
	acceleration = cocos2d::Vec2(0, 0);
	gravity = cocos2d::Vec2(0, -gravStrength);
	radius = playerSprite->getBoundingBox().size.width / 2;
	canJump = true;
	width = playerSprite->getBoundingBox().size.width;
	height = playerSprite->getBoundingBox().size.height;
	footPos = Vec2(position.x, position.y - (height / 2));

	setHitBox(Rect(position.x - width / 2, position.y - height / 2, width, height));

	if (orientation == 1) {
		attackBox = Rect(position.x + width / 2, position.y - width / 4, width, height / 2);
	}
	else if (orientation == -1) {
		attackBox = Rect(position.x + width / 2, position.y - width / 4, width, height / 2);
	}
	isStuned = false;
	isAttacking = false;
	attackButtonPressed = false;
	
	//initialize the sound system, this is where the volume level is set from the options menu, for now it is hard coded at 0.5f	
	sound_vol.setVolume(0.5f);
	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, nullptr);
	return true;
}
void Player::loadAnimations() {
	//TODO: Scale Image down
	char str[100] = { 0 };
	if (playerNumber == 1) {
		//Loading jumping animation
		for (int i = 1; i <= NUM_JUMP_FRAMES; i++) {
			sprintf(str, "/PlayerAnimation/player1/jump/jump_%i.png", i);
			auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 205, 307), false, Vec2(0, 0), Size(205, 307));
			JumpAnimation.pushBack(frame);
		}
		auto tmpAnimation = Animation::createWithSpriteFrames(JumpAnimation, 0.12f);
		jumpAnimate = Animate::create(tmpAnimation);
		jumpAnimate->retain();
		jumping = jumpAnimate;

		//Loading Walking animation
		for (int i = 1; i <= NUM_WALK_FRAMES; i++) {
			sprintf(str, "/PlayerAnimation/player1/walk/Walking_%i.png", i);
			auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 369, 572), false, Vec2(0, 0), Size(369, 572));
			walkAnimation.pushBack(frame);
		}
		tmpAnimation = Animation::createWithSpriteFrames(walkAnimation, 0.09f);
		walkAnimate = Animate::create(tmpAnimation);
		walkAnimate->retain();
		walking = walkAnimate;

		//Loading idle animation
		//Will fully implement later
		for (int i = 1; i <= NUM_IDLE_FRAMES; i++) {
			sprintf(str, "/PlayerAnimation/player1/hit_reaction/react_%i.png", i);
			auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 124, 202), false, Vec2(0, 0), Size(124, 202));
			idleAnimation.pushBack(frame);
		}
		tmpAnimation = Animation::createWithSpriteFrames(idleAnimation, 0.09f);
		idleAnimate = Animate::create(tmpAnimation);
		idleAnimate->retain();
		idling = idleAnimate;

		for (int i = 1; i < NUM_ATTACK_FRAMES; i++) {
			sprintf(str, "/PlayerAnimation/player1/attack/punching_%i.png", i);
			auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 254, 310), false, Vec2(0, 0), Size(254, 310));
			attackAnimation.pushBack(frame);
		}
		tmpAnimation = Animation::createWithSpriteFrames(attackAnimation, 0.09f);
		attackAnimate = Animate::create(tmpAnimation);
		attackAnimate->retain();
		attacking = attackAnimate;
	}
	else {
		//Loading jumping animation
		for (int i = 1; i <= NUM_JUMP_FRAMES; i++) {
			sprintf(str, "/PlayerAnimation/player2/jump/jump_%i.png", i);
			auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 205, 307), false, Vec2(0, 0), Size(205, 307));
			JumpAnimation.pushBack(frame);
		}
		auto tmpAnimation = Animation::createWithSpriteFrames(JumpAnimation, 0.12f);
		jumpAnimate = Animate::create(tmpAnimation);
		jumpAnimate->retain();
		jumping = jumpAnimate;

		//Loading Walking animation
		for (int i = 1; i <= NUM_WALK_FRAMES; i++) {
			sprintf(str, "/PlayerAnimation/player2/walk/Walking_%i.png", i);
			auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 369, 572), false, Vec2(0, 0), Size(369, 572));
			walkAnimation.pushBack(frame);
		}
		tmpAnimation = Animation::createWithSpriteFrames(walkAnimation, 0.09f);
		walkAnimate = Animate::create(tmpAnimation);
		walkAnimate->retain();
		walking = walkAnimate;

		//Loading idle animation
		//Will fully implement later
		for (int i = 1; i <= NUM_IDLE_FRAMES; i++) {
			sprintf(str, "/PlayerAnimation/player2/hit_reaction/react_%i.png", i);
			auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 124, 202), false, Vec2(0, 0), Size(124, 202));
			idleAnimation.pushBack(frame);
		}
		tmpAnimation = Animation::createWithSpriteFrames(idleAnimation, 0.09f);
		idleAnimate = Animate::create(tmpAnimation);
		idleAnimate->retain();
		idling = idleAnimate;

		for (int i = 1; i < NUM_ATTACK_FRAMES; i++) {
			sprintf(str, "/PlayerAnimation/player2/attack/punching_%i.png", i);
			auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 254, 310), false, Vec2(0, 0), Size(254, 310));
			attackAnimation.pushBack(frame);
		}
		tmpAnimation = Animation::createWithSpriteFrames(attackAnimation, 0.09f);
		attackAnimate = Animate::create(tmpAnimation);
		attackAnimate->retain();
		attacking = attackAnimate;
		

	}
}

void Player::update(float dt) { // dt is in seconds
	isAttacking = false;
	if (isStuned) {
		velocity = Vec2(0, 0);
		acceleration = Vec2(0, 0);
	}
	if (hitDeathPlane(position)) {
		//stop any old death audio to avoid stacking sounds.	
		death_channel->stop();
		//file path	
		std::string death_path = FileUtils::getInstance()->fullPathForFilename("audio/death.wav");
		//create sound and set the volume to whatever the volume level is set to	
		system->createSound(death_path.c_str(), FMOD_LOOP_OFF, 0, &death_sound);
		system->playSound(death_sound, 0, true, &death_channel);
		death_channel->setVolume(sound_vol.getVolume());
		//play the sound	
		death_channel->setPaused(false);
		if (playerLives == 2) {
			
			playerDamageIcon3->setVisible(false);
			playerDamageIcon2->setVisible(true);
		}
		if (playerLives == 1) {
			playerDamageIcon2->setVisible(false);
			playerDamageIcon1->setVisible(true);
		}
		damageLabel->setString("0.0%");
		reset();
		return;
	}
	if (onCooldown) {
		comboCooldown -= dt*10;
		if (comboCooldown <= 0) {
			onCooldown = false;
			comboCooldown = comboCooldownTime;
		}
	}
	auto isInAir = InAir(footPos);
	if (InAir(footPos)) {
		onGround = false;
		acceleration = cocos2d::Vec2(0, 0);
		// check if on screen
		if (position.x < 0 + (width / 2) || position.x > boundingBox.getMaxX() - (width / 2)) {
			reset();
			return;
		}
		if (position.y < 0 + (height / 2) || position.y > boundingBox.getMaxY() - (height / 2)) {
			reset();
			return;
		}
	}

	else {
		onGround = true;
		//Temp statement 
		playerSprite->stopActionByTag(JUMP_TAG);
		jumping->setTag(NULL_TAG);
		if (abs(velocity.x) <=0.1 && abs(acceleration.x) == 0 && playerSprite->getActionByTag(ATTACK_TAG)==nullptr) {
			playerSprite->stopAllActions();
			walking->setTag(NULL_TAG);
			if (playerSprite->getActionByTag(IDLE_TAG) == nullptr) {
				idling->setTag(IDLE_TAG);
				playerSprite->setScale(SPRITE_SCALE * 2.8);
				playerSprite->runAction(idling);
				
			}	
		}
	}
	int presentP1 = 0;
	int presentP2 = 0;

	if (playerNumber == 1) {
		presentP1 = glfwJoystickPresent(GLFW_JOYSTICK_1);
	}
	else if (playerNumber == 2) {
		presentP2 = glfwJoystickPresent(GLFW_JOYSTICK_2);
	}

	int axesCount = NULL;
	const float* axes = nullptr;
	int buttonCount = NULL;
	const unsigned char* buttons = nullptr;
	const char* name = nullptr;

	if (presentP1) {
		if (!isStuned) {
			axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
			buttonCount;
			buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
			name = glfwGetJoystickName(GLFW_JOYSTICK_1);
		}
	} else if (presentP2) {
		if (!isStuned) {
			axesCount;
			axes = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &axesCount);
			buttonCount;
			buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &buttonCount);
			name = glfwGetJoystickName(GLFW_JOYSTICK_2);
		}
	}

	if (presentP1 || presentP2) {
		/*
		CCLOG("Axes Count: %d", axesCount);
		CCLOG("Left Stick Hori: %.2f", axes[0]);
		CCLOG("Left Stick Vert: %.2f", axes[1]);
		CCLOG("Right Stick Hori: %.2f", axes[2]);
		CCLOG("Right Stick Vert: %.2f", axes[3]);
		CCLOG("Left Trigger: %.2f", axes[4]);
		CCLOG("Right Trigger: %.2f", axes[5]);

		CCLOG("Button Count: %d", buttonCount);
		CCLOG("A: %d", buttons[0] == GLFW_PRESS);
		CCLOG("B: %d", buttons[1] == GLFW_PRESS);
		CCLOG("X: %d", buttons[2] == GLFW_PRESS);
		CCLOG("Y: %d", buttons[3] == GLFW_PRESS);
		CCLOG("LB: %d", buttons[4] == GLFW_PRESS);
		CCLOG("RB: %d", buttons[5] == GLFW_PRESS);
		CCLOG("Select: %d", buttons[6] == GLFW_PRESS);
		CCLOG("Start: %d", buttons[7] == GLFW_PRESS);
		CCLOG("LS: %d", buttons[8] == GLFW_PRESS);
		CCLOG("RS: %d", buttons[9] == GLFW_PRESS);
		CCLOG("UP: %d", buttons[10] == GLFW_PRESS);
		CCLOG("RT: %d", buttons[11] == GLFW_PRESS);
		CCLOG("DN: %d", buttons[12] == GLFW_PRESS);
		CCLOG("LF: %d", buttons[13] == GLFW_PRESS);
		*/
		//Moving right
		if (axes[LS_HORI] > .15) {
			acceleration.x = 3 * axes[0];
			orientation = 1;
			playerSprite->stopActionByTag(IDLE_TAG);
			idling->setTag(NULL_TAG);
			if (playerSprite->getActionByTag(1) == nullptr) {
				walking->setTag(WALK_TAG);
				playerSprite->runAction(walking);
				playerSprite->setScaleX(SPRITE_SCALE*orientation);
				playerSprite->setScaleY(SPRITE_SCALE);
				//playerSprite->setScale(0.3);

			}
		}

		//Moving Left
		else if (axes[LS_HORI] < -.15) {
			acceleration.x = 3 * axes[0];
			orientation = -1;
			playerSprite->stopActionByTag(IDLE_TAG);
			idling->setTag(NULL_TAG);
			if (playerSprite->getActionByTag(WALK_TAG) == nullptr) {
				walking->setTag(WALK_TAG);
				playerSprite->runAction(walking);
				playerSprite->setScaleX(SPRITE_SCALE*orientation);
				playerSprite->setScaleY(SPRITE_SCALE);
				//playerSprite->setScaleX(-0.3);	
				//playerSprite->setScaleY(0.3);

			}
		}
		else {
			acceleration.x *= 0.1;
		}
		if (buttons[A] == GLFW_PRESS) {
			if (canJump) {
				//stop previous jump sounds	
				jump_channel->stop();
				//file path	
				std::string jump_path = FileUtils::getInstance()->fullPathForFilename("audio/jump.wav");
				//create the sound, initted as paused	
				system->createSound(jump_path.c_str(), FMOD_LOOP_OFF, 0, &jump_sound);
				system->playSound(jump_sound, 0, true, &jump_channel);
				//set volume	
				jump_channel->setVolume(sound_vol.getVolume());
				//unpaused and play sound	
				jump_channel->setPaused(false);
				acceleration.y += 200;
				canJump = false;
				playerSprite->stopActionByTag(IDLE_TAG);
				idling->setTag(NULL_TAG);
				if (playerSprite->getActionByTag(JUMP_TAG) == nullptr) {
					jumping->setTag(JUMP_TAG);
					playerSprite->runAction(jumping);
					playerSprite->setScaleY(SPRITE_SCALE * 2.5);
					playerSprite->setScaleX(SPRITE_SCALE*orientation* 2.5);
					//playerSprite->setScale(0.3);
				}

			}
		}
		if (buttons[B] == GLFW_PRESS) {
			if (canJump) {
				acceleration.x = 0;
				velocity.x *= .5;
				idling->setTag(NULL_TAG);
			}
		}
		if (buttons[Y] == GLFW_PRESS) {
			CCLOG("Y is Pressed");
			isAttacking = true;
			attackButtonPressed = true;
			//stop old attack sounds	
			attack_channel->stop();
			//file path	
			std::string attack_path = FileUtils::getInstance()->fullPathForFilename("audio/attack_miss.wav");
			//create sound	
			system->createSound(attack_path.c_str(), FMOD_LOOP_OFF, 0, &attack_sound);
			system->playSound(attack_sound, 0, true, &attack_channel);
			//set volume	
			attack_channel->setVolume(sound_vol.getVolume());
			//play	
			playerSprite->stopActionByTag(IDLE_TAG);
			idling->setTag(NULL_TAG);
			attack_channel->setPaused(false);
			if (playerSprite->getActionByTag(ATTACK_TAG) == nullptr) {
				attacking->setTag(ATTACK_TAG);
				playerSprite->runAction(attacking);
				playerSprite->setScaleX(0.6 * orientation);
				playerSprite->setScaleY(0.6);

			}
		}
		if (buttons[X] == GLFW_PRESS) {
			CCLOG("X is Pressed");
			if (!hasItem) {
				isPickingUp = true;
			}
		}
		else {
			isPickingUp = false;
		}

	}

	if (onGround) {
		if (velocity.y < 0) {
			acceleration.y = 0;
			velocity.y = 0;
			setFootPos(Vec2(footPos.x, int(footPos.y) + int(_CurMap->getTileSize().height) - (int(footPos.y) % int(_CurMap->getTileSize().height))));
			canJump = true;
		}
		if (acceleration.x == 0) {
			velocity.x *= .8;
		}
		velocity += acceleration;
	}
	else {
		velocity += acceleration + gravity * dt;
	}

	if (orientation == 1) {
		setAttackBox(Rect(position.x + width / 2, position.y - width / 4, width, height / 2));
	}
	else {
		setAttackBox(Rect(position.x - 3 * (width / 2), position.y - width / 4, width, height / 2));
	}
	setHitBox(Rect(position.x - width / 2, position.y - height / 2, width, height));
	position += velocity * dt;
	footPos = Vec2(position.x, position.y - (height / 2));
	playerSprite->setPosition(position);
}
bool Player::hitDeathPlane(Vec2 currentPosition) {
	Vec2 tileCoord = tileCoordForPosition(currentPosition);
	int tileGid = _DeathPlane->getTileGIDAt(tileCoord);
	if (tileGid) {
		damage = 0.0;
		playerLives -= 1;
		return true;
	}
	return false;
}
bool Player::ComboChain(float dtF, float dtI) {
	//Only works if the next attack input happens two seconds or less after the previous
	if (abs(dtI-comboStartTime) <0.05 && numTimesAttacked<=3 && attackButtonPressed) {
		attackButtonPressed = false;
		numTimesAttacked += 1;
		CCLOG("Num times attcked %u", numTimesAttacked);
		return true;
	}

	attackButtonPressed = false;
	numTimesAttacked = 0;
	return false;
}

bool Player::InAir(Vec2 Currentposition) {
	Vec2 tileCoord = tileCoordForPosition(Currentposition);
	int tileGid = _ground->getTileGIDAt(tileCoord);
	if (tileGid) {
		return false;
	}

	return true;
}

void Player::setHitBox(Rect newBox) {
	hitBox = newBox;
}

void Player::setAttackBox(Rect newBox) {
	attackBox = newBox;
}


void Player::setFootPos(Vec2 newPos) {
	footPos = newPos;
	position = Vec2(footPos.x, footPos.y + (height / 2));
}


void Player::updateStunStatus() {
	if (!isStuned) {
		isStuned= true;
	}
	else {
		isStuned = false;
	}
	
}
bool Player::Attacked() {
	attackButtonPressed = true;
	return isAttacking;
}

//Converts pixel position to tile coordinate on tile map
Vec2 Player::tileCoordForPosition(Vec2 CurrentPosition) {
	int x = CurrentPosition.x / _CurMap->getTileSize().width; // tile x coord	
	int y = ((_CurMap->getMapSize().height * _CurMap->getTileSize().height) - CurrentPosition.y) / _CurMap->getTileSize().height; // tile y coord	
	return cocos2d::Vec2(x, y); // return tile coords
}

//Resets player attributes
void Player::reset() {
	position = Spawnpoint;
	playerSprite->setPosition(position);
	velocity = Vec2(0, 0);
	acceleration = Vec2(0, 0);
	canJump = true;
}

void Player::setItem(Item* newItem) {
	item = newItem;
}