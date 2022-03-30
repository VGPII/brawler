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

	gravity = 400;

	_MainMap = TMXTiledMap::create("mainMap.tmx");
	auto background = _MainMap->getLayer("Background");
	_ground = _MainMap->getLayer("Collision");
	_ground->setVisible(false);
	_DeathPlane = _MainMap->getLayer("Death_Plane");
	_DeathPlane->setVisible(false);
	this->addChild(_MainMap);

	playerOne = new Player();
	playerTwo = new Player();

	playerTwo->init(gravity, _MainMap, this->getBoundingBox());
	playerOne->init(gravity, _MainMap, this->getBoundingBox());

	playerTwo->playerSprite = Sprite::create("ball_blue.png");
	playerTwo->position.x += 100;

	this->addChild(playerOne->playerSprite);
	this->addChild(playerTwo->playerSprite);

	this->scheduleUpdate();

	return true;
}

void BallBounce::update(float dt) {
	CCLOG("Player Position %f", playerOne->position.x);
	CCLOG("Enemy Position %f", playerTwo->position.x);

	playerOne->update(dt);
	playerTwo->update(dt);
	checkForCollision(playerOne->position, playerTwo->position);
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

void BallBounce::checkForCollision(Vec2 AttackerPos, Vec2 ReciverPos) {
	if (AttackerPos.x == ReciverPos.x || abs(AttackerPos.x - ReciverPos.x) < 10) {// Change this value to be the radius of the sprite
		if (AttackerPos.y == ReciverPos.y || abs(AttackerPos.y - ReciverPos.y) < 10) {
			CCLOG("Collison");
			//Insert momentum calculations here
		}
	}


}