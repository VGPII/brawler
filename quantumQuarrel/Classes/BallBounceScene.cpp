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
#include <math.h>

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

Scene* BallBounce::createScene()
{
	return BallBounce::create();
}

// on "init" you need to initialize your instance
bool BallBounce::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	
	_MainMap = TMXTiledMap::create("mainMap.tmx");
	auto background = _MainMap->getLayer("Background");
	_ground = _MainMap->getLayer("Collision");
	this->addChild(_MainMap);
	objectGroup = _MainMap->getObjectGroup("SpawnPoints");

	if (objectGroup == NULL) {
		CCLOG("Tile map does not have an object layer");
	}
	ValueMap spawnPoint = objectGroup->getObject("SpawnPointP1");
	int x = spawnPoint.at("x").asInt();
	int y = spawnPoint.at("y").asInt();

	ballSprite = Sprite::create("ball.png");
	
	//ballSprite->setPosition(Point((visibleSize.width / 2) + origin.x, (visibleSize.height / 2) + origin.y));
	ballSprite->setPosition(Vec2(x, y));
	this->setViewPointCenter(ballSprite->getPosition());
	this->addChild(ballSprite);
	position = ballSprite->getPosition();
	velocity = cocos2d::Vec2(0, 0);
	acceleration = cocos2d::Vec2(0, 0);
	gravity = cocos2d::Vec2(0, -400);
	radius = ballSprite->getBoundingBox().size.width / 2;
	canJump = true;

	

	this->scheduleUpdate();

	return true;
}

void BallBounce::update(float dt) {
	if (InAir(position)) {
		onGround = false;
		acceleration = cocos2d::Vec2(0, 0);
		if (position.x < 0 + radius || position.x > this->getBoundingBox().getMaxX() - radius) {
			velocity.x *= 0;
			if (position.x < 0 + radius) {
				position.x = 0 + radius;
			}
			else {
				position.x = this->getBoundingBox().getMaxX() - radius;
			}
		}
		if (position.y < 0 + radius || position.y > this->getBoundingBox().getMaxY() - radius) {
			canJump = true;
			velocity.y *= 0;
			if (position.y < 0 + radius) {
				position.y = 0 + radius;
			}
			else {
				position.y = this->getBoundingBox().getMaxY() - radius;
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
	}
	if (onGround) {
		gravity = Vec2(0,0);
	}
	velocity += acceleration + gravity * dt;
	

	position += velocity * dt;
	ballSprite->setPosition(position);
	//this->setViewPointCenter(position);
}
void BallBounce::setViewPointCenter(Vec2 Position) {
	Size winSize = cocos2d::Director::getInstance()->getWinSize(); // Getting the window size
	//These functions Make sure the camera stops following the player once they get "Off screen"
	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	x = MIN(x, (_MainMap->getMapSize().width * this->_MainMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (_MainMap->getMapSize().height * _MainMap->getTileSize().height) - winSize.height / 2);

	Vec2 actualposition = Vec2(x, y);
	Vec2 centerOfView = Vec2(winSize.width / 2, winSize.height / 2);
	Vec2 viewPoint = centerOfView;
	viewPoint.subtract(actualposition);

	this->setPosition(viewPoint);
}
Vec2 BallBounce::tileCoordForPosition(Vec2 CurrentPosition) {
	CurrentPosition = Vec2(CurrentPosition.x*2, CurrentPosition.y*2);
	int x = CurrentPosition.x / _MainMap->getTileSize().width;
	int y = ((_MainMap->getMapSize().height * _MainMap->getTileSize().height) - CurrentPosition.y) / _MainMap->getTileSize().height;
	return Vec2(x, y);
}

bool BallBounce::InAir(Vec2 Currentposition){
	Vec2 tileCoord = tileCoordForPosition(position);
	int tileGid = _ground->getTileGIDAt(tileCoord);
	if (tileGid) {
		ValueMap properties = _MainMap->getPropertiesForGID(tileGid).asValueMap();
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



