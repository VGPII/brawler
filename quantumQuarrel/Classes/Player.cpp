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

bool Player::init(int gravStrength, TMXTiledMap* initMap, Rect initBoundingBox)
{
	boundingBox = initBoundingBox;
	playerSprite = Sprite::create("ball.png");
	_CurMap = initMap;
	_background = _CurMap->getLayer("Background");
	_ground = _CurMap->getLayer("Collision");
	_DeathPlane = _CurMap->getLayer("Death_Plane");
	objectGroup = _CurMap->getObjectGroup("SpawnPoints");


	if (objectGroup == NULL) {
		CCLOG("Tile map does not have an object layer");
	}
	ValueMap spawnPoint = objectGroup->getObject("SpawnPointP1");
	SpawnpointP1 = Vec2(spawnPoint.at("x").asInt(), spawnPoint.at("y").asInt());

	position = SpawnpointP1;
	velocity = cocos2d::Vec2(0, 0);
	acceleration = cocos2d::Vec2(0, 0);
	gravity = cocos2d::Vec2(0, -gravStrength);
	radius = playerSprite->getBoundingBox().size.width / 2;
	canJump = true;

	return true;
}

void Player::update(float dt) {
	if (hitDeathPlane(position)) {
		position = SpawnpointP1;
		playerSprite->setPosition(position);
		velocity = Vec2(0, 0);
		acceleration = Vec2(0, 0);
		canJump = true;

		return;
	}
	if (InAir(position)) {
		onGround = false;
		acceleration = cocos2d::Vec2(0, 0);
		if (position.x < 0 + radius || position.x > boundingBox.getMaxX() - radius) {
			velocity.x *= 0;
			if (position.x < 0 + radius) {
				position.x = 0 + radius;
			}
			else {
				position.x = boundingBox.getMaxX() - radius;
			}
		}
		if (position.y < 0 + radius || position.y > boundingBox.getMaxY() - radius) {
			canJump = true;
			velocity.y *= 0;
			if (position.y < 0 + radius) {
				position.y = 0 + radius;
			}
			else {
				position.y = boundingBox.getMaxY() - radius;
			}
		}
	}
	else {
		onGround = true;
	}



	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (present == 1) {
		int axesCount;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		int buttonCount;
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		const char *name = glfwGetJoystickName(GLFW_JOYSTICK_1);
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

		if (axes[LS_HORI] > .15) {
			acceleration.x = 3 * axes[0];
		}
		if (axes[LS_HORI] < -.15) {
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
	}
	if (onGround) {
		if (velocity.y < 0) {
			acceleration.y = 0;
			velocity.y = 0;
			canJump = true;
		}
		velocity += acceleration;
	}
	else {
		velocity += acceleration + gravity * dt;
	}

	position += velocity * dt;
	
	playerSprite->setPosition(position);
}

bool Player::hitDeathPlane(Vec2 currentPosition) {
	Vec2 tileCoord = tileCoordForPosition(currentPosition);
	int tileGid = _DeathPlane->getTileGIDAt(tileCoord);
	if (tileGid) {
		//ValueMap properties = _MainMap->getPropertiesForGID(tileGid).asValueMap();
		//if (properties.size() > 0) {
		//	std::string plane;

			//plane = properties.at("Collidable").asString();
			//CCLOG("Props %S",properties.at("Death_Plane"));
			//if (plane.compare("True")) {
		return true;
	}
	//}
//}

	return false;

}

bool Player::InAir(Vec2 Currentposition) {
	Vec2 tileCoord = tileCoordForPosition(Currentposition);
	int tileGid = _ground->getTileGIDAt(tileCoord);
	if (tileGid) {
		ValueMap properties = _CurMap->getPropertiesForGID(tileGid).asValueMap();
		if (properties.size() > 0) {
			std::string collision;
			collision = properties.at("Collidable").asString();
			if (collision.compare("True")) {
				return false;
			}
		}
	}

	return true;
}

Vec2 Player::tileCoordForPosition(Vec2 CurrentPosition) {
	CurrentPosition = Vec2(CurrentPosition.x * 2, CurrentPosition.y * 2);
	int x = CurrentPosition.x / _CurMap->getTileSize().width;
	int y = ((_CurMap->getMapSize().height * _CurMap->getTileSize().height) - CurrentPosition.y) / _CurMap->getTileSize().height;
	return Vec2(x, y);
}