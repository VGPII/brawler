#include "Player.h"

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
#define WALK_TAG 1
#define IDLE_TAG 0
#define NULL_TAG -1
bool Player::init(int gravStrength, TMXTiledMap* initMap, Rect initBoundingBox, int playerNumberInit)
{
	boundingBox = initBoundingBox;
	_CurMap = initMap;
	_background = _CurMap->getLayer("Background");
	_ground = _CurMap->getLayer("Collision");
	_DeathPlane = _CurMap->getLayer("Death_Plane");
	NUM_IDLE_FRAMES = 1;
	NUM_JUMP_FRAMES = 8;
	NUM_WALK_FRAMES = 6;
	playerLives = 3;
	damage = 0.0;
	comboCooldownTime = 0.5;
	comboCooldown = comboCooldownTime;
	onCooldown = false;


	objectGroup = _CurMap->getObjectGroup("SpawnPoints");
	playerNumber = playerNumberInit;

	if (objectGroup == NULL) {
		CCLOG("Tile map does not have an object layer");
	}
	
	if (playerNumber == 1) {
		ValueMap spawnPoint = objectGroup->getObject("SpawnPointP1");
		Spawnpoint = Vec2(spawnPoint.at("x").asInt() * _CurMap->getScaleX(), spawnPoint.at("y").asInt()* _CurMap->getScaleY());
		orientation = 1; // Facing towards the right side of the screen
		playerSprite = Sprite::create("ball.png");
		playerSprite->setVisible(true);
		//Will need a loop to instantiate all of the animation types
		loadAnimations();
	}
	else if (playerNumber == 2) {
		ValueMap spawnPoint = objectGroup->getObject("SpawnPointP2");
		Spawnpoint = Vec2(spawnPoint.at("x").asInt() * _CurMap->getScaleX(), spawnPoint.at("y").asInt() * _CurMap->getScaleY());
		orientation = -1; // Facing towards the left side of the screen
		playerSprite = Sprite::create("ball_blue.png");

	}
	
	

	position = Spawnpoint;
	velocity = cocos2d::Vec2(0, 0);
	acceleration = cocos2d::Vec2(0, 0);
	gravity = cocos2d::Vec2(0, -gravStrength);
	radius = playerSprite->getBoundingBox().size.width / 2;
	width = playerSprite->getBoundingBox().size.width;
	height = playerSprite->getBoundingBox().size.height;
	footPos = Vec2(position.x, position.y - (height / 2));
	canJump = true;

	setHitBox(Rect(position.x - radius, position.y - radius, 2 * radius, 2 * radius));

	if (orientation == 1) {
		attackBox = Rect(position.x + 10, position.y, 10, 10);
	}
	else if (orientation == -1) {
		attackBox = Rect(position.x - 10, position.y, -10, 10);
	}
	isStuned = false;
	isAttacking = false;
	attackButtonPressed = false;
	//Setting number of animation frames
	

	return true;
}
void Player::loadAnimations() {
	//TODO: Scale Image down
	char str[100] = { 0 };
	//Loading jumping animation
	for (int i = 1; i <= NUM_JUMP_FRAMES; i++) {
		sprintf(str, "/PlayerAnimation/Jump/jumping_animation_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 221, 572), false, Vec2(0, 300), Size(221, 572));

		JumpAnimation.pushBack(frame);
	}
	auto tmpAnimation = Animation::createWithSpriteFrames(JumpAnimation, 0.12f);
	jumpAnimate = Animate::create(tmpAnimation);
	jumpAnimate->retain();
	jumping = jumpAnimate;
	
	//Loading Walking animation
	for (int i = 1; i <= NUM_WALK_FRAMES; i++) {
		sprintf(str, "/PlayerAnimation/walk/Walking_animation_%i.png", i);
		auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 200, 609), false, Vec2(0, 300), Size(200, 609));
		walkAnimation.pushBack(frame);
	}
	tmpAnimation = Animation::createWithSpriteFrames(walkAnimation, 0.09f);
	walkAnimate = Animate::create(tmpAnimation);
	walkAnimate->retain();
	walking = walkAnimate;
	
	//Loading idle animation
	//Will fully implement later
	for (int i = 1; i <= NUM_IDLE_FRAMES; i++) {
		sprintf(str, "ball.png", i);
		auto frame = SpriteFrame::create(str, Rect(position.x, position.y, 32, 32), false, Vec2(0, 0), Size(32, 32));
		idleAnimation.pushBack(frame);
	}

	tmpAnimation = Animation::createWithSpriteFrames(idleAnimation, 0.09f);
	idleAnimate = Animate::create(tmpAnimation);
	idleAnimate->retain();
	idling = idleAnimate;
	

}

void Player::update(float dt) { // dt is in seconds
	isAttacking = false;
	
	if (isStuned) {
		velocity = Vec2(0, 0);
		acceleration = Vec2(0, 0);
	}
	if (hitDeathPlane(position)) {
		position = Spawnpoint;
		playerSprite->setPosition(position);
		velocity = Vec2(0, 0);
		acceleration = Vec2(0, 0);
		canJump = true;
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
	if (InAir(footPos)) { // check foot position
		onGround = false;
		acceleration = cocos2d::Vec2(0, 0);
		//check if on screen
		if (position.x < 0 + (width / 2) || position.x > boundingBox.getMaxX() - (width/2)) {
			velocity.x *= 0;
			if (position.x < 0 + (width / 2)) {
				position.x = 0 + (width / 2);
			}
			else {
				position.x = boundingBox.getMaxX() - (width / 2);
			}
		}
		if (position.y < 0 + (height / 2) || position.y > boundingBox.getMaxY() - (height / 2)) {
			canJump = true;
			velocity.y *= 0;
			if (position.y < 0 + (height / 2)) {
				position.y = 0 + (height / 2);
			}
			else {
				position.y = boundingBox.getMaxY() - (height / 2);
			}
		}
	}
	else {
		onGround = true;
		//Temp statement 
		if (playerNumber == 1) {
			playerSprite->stopActionByTag(3);
			jumping->setTag(NULL_TAG);
			if (velocity.x == 0 && acceleration.x == 0) {
				playerSprite->stopActionByTag(1);
				walking->setTag(NULL_TAG);
				if (playerSprite->getActionByTag(IDLE_TAG) == nullptr) {
					idling->setTag(IDLE_TAG);
					//playerSprite->runAction(idling);
					playerSprite->setScale(1);
				}

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

	if (presentP1 == 1) {
		if(!isStuned){
		int axesCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		int buttonCount;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);

		//Moving right
		if (axes[LS_HORI] > .15) {
			acceleration.x = 3 * axes[0];
			orientation = 1;
			if (playerSprite->getActionByTag(1) == nullptr) {
				walking->setTag(WALK_TAG);
				//playerSprite->runAction(walking);
				//playerSprite->setScale(0.3);
			}
		}

		//Moving Left
		else if (axes[LS_HORI] < -.15) {
			acceleration.x = 3 * axes[0];
			orientation = -1;
			if (playerSprite->getActionByTag(1) == nullptr) {
				walking->setTag(WALK_TAG);
				//playerSprite->runAction(walking);
				//playerSprite->setScaleX(-0.3);
				//playerSprite->setScaleY(0.3);
			}

		}
		else {
			acceleration.x = 0;
		}
		if (buttons[A] == GLFW_PRESS) {
			if (canJump) {
				acceleration.y += 200;
				canJump = false;
				if (playerSprite->getActionByTag(3) == nullptr) {
					jumping->setTag(JUMP_TAG);
					//playerSprite->runAction(jumping);
					//playerSprite->setScale(0.3);
				}

			}
		}
		if (buttons[B] == GLFW_PRESS) {
			if (canJump) {
				acceleration.x = 0;
				velocity.x *= .5;
			}
		}
		if (buttons[Y] == GLFW_PRESS) {
			isAttacking = true;
			attackButtonPressed = true;
		}
	}
}
	if (presentP2) {
		if (!isStuned) {
			int axesCount;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &axesCount);
			int buttonCount;
			const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &buttonCount);
			const char* name = glfwGetJoystickName(GLFW_JOYSTICK_2);
			
			if (axes[LS_HORI] > .15) {
				orientation = 1;
				acceleration.x = 3 * axes[0];
			}
			if (axes[LS_HORI] < -.15) {
				orientation = -1;
				acceleration.x = 3 * axes[0];
			}
			if (buttons[A] == GLFW_PRESS) {
				if (canJump) {
					acceleration.y += 200;
					canJump = false;
				}
			}
			if (buttons[B] == GLFW_PRESS) {
				if (canJump) {
					acceleration.x = 0;
					velocity.x *= .8;
				}
			}
			if (buttons[Y] == GLFW_PRESS) {
				isAttacking = true;
				attackButtonPressed = true;

			}
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
			setAttackBox(Rect(position.x + 10, position.y, 10, 10));
		}
		else {
			setAttackBox(Rect(position.x - 10, position.y, -10, 10));
		}
		setHitBox(Rect(position.x - radius, position.y - radius, 2 * radius, 2 * radius));

		position += velocity * dt;
		footPos = Vec2(position.x, position.y - (height / 2));
		playerSprite->setPosition(position);
	
}
bool Player::hitDeathPlane(Vec2 currentPosition) {
	Vec2 tileCoord = tileCoordForPosition(currentPosition);
	int tileGid = _DeathPlane->getTileGIDAt(tileCoord);
	if (tileGid) {
		damage = 0.0;
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

Vec2 Player::tileCoordForPosition(Vec2 CurrentPosition) {
	int x = CurrentPosition.x / _CurMap->getTileSize().width; // tile x coord
	int y = ((_CurMap->getMapSize().height * _CurMap->getTileSize().height) - CurrentPosition.y) / _CurMap->getTileSize().height; // tile y coord
	return cocos2d::Vec2(x, y); // return tile coords
}