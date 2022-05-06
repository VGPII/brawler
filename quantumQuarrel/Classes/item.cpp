/****************************************************************************
Copyright(c) 2017 - 2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "Item.h"
#include <math.h>

USING_NS_CC;

#define SHIELD 0
#define SWORD 1


bool Item::init(int gravStrength, int initType, TMXTiledMap* initMap) {
	type = initType;
	_CurMap = initMap;
	_ground = _CurMap->getLayer("Collision");
	objectGroup = _CurMap->getObjectGroup("SpawnPoints");
	idleDuration = 15.0;
	// Set Sprite and item properties
	if (type == SHIELD) {
		itemSprite = Sprite::create("/itemAssets/shield.png");
		buffDuration = 10.0;
	}
	else if (type == SWORD) {
		itemSprite = Sprite::create("/itemAssets/sword.png");
		buffDuration = 10.0;
	}
	itemSprite->setScale(0.5);
	// Position to spawn at
	ValueMap itemSpawnPoint = objectGroup->getObject("itemSpawnPoint");
	Spawnpoint = Vec2(itemSpawnPoint.at("x").asInt() * _CurMap->getScaleX(), itemSpawnPoint.at("y").asInt() * _CurMap->getScaleY());
	position = Spawnpoint;
	velocity = cocos2d::Vec2(0, 0);
	acceleration = cocos2d::Vec2(0, 0);
	gravity = cocos2d::Vec2(0, -gravStrength);
	width = itemSprite->getBoundingBox().size.width;
	height = itemSprite->getBoundingBox().size.height;
	basePos = Vec2(position.x, position.y - (height / 2));

	isEquiped = false;
	setToDespawn = false;
	return true;
}

void Item::update(float dt) {
	if (!isEquiped) {
		// Adjust timer
		if (idleDuration <= 0) {
			setToDespawn = true;
		}
		else {
			idleDuration -= dt;
		}
	}
	else {
		// Hide and move item out of the way
		gravity = Vec2(0, 0);
		position = Vec2(width, height);
		itemSprite->setVisible(false);
		// Adjust timer
		if (buffDuration <= 0) {
			setToDespawn = true;
		}
		else {
			buffDuration -= dt;
		}
	}
	// Physics calculations
	if (!inAir(basePos)) {
		if (velocity.y < 0) {
			acceleration.y = 0;
			velocity.y = 0;
			setBasePos(Vec2(basePos.x, int(basePos.y) + int(_CurMap->getTileSize().height) - (int(basePos.y) % int(_CurMap->getTileSize().height))));
		}
		velocity += acceleration;
	}
	else {
		velocity += acceleration + gravity * dt;
	}

	hitBox = Rect(position.x - width / 2, position.y - height / 2, width, height);
	position += velocity * dt;
	basePos = Vec2(position.x, position.y - (height / 2));
	itemSprite->setPosition(position);
}

void Item::pickUp() {
	isEquiped = true;
}

void Item::setBasePos(Vec2 newPos) {
	basePos = newPos;
	position = Vec2(basePos.x, basePos.y + (height / 2));
}

bool Item::inAir(Vec2 Currentposition) {
	Vec2 tileCoord = tileCoordForPosition(Currentposition);
	int tileGid = _ground->getTileGIDAt(tileCoord);
	if (tileGid) {
		return false;
	}
	return true;
}

Vec2 Item::tileCoordForPosition(Vec2 CurrentPosition) {
	int x = CurrentPosition.x / _CurMap->getTileSize().width; // tile x coord	
	int y = ((_CurMap->getMapSize().height * _CurMap->getTileSize().height) - CurrentPosition.y) / _CurMap->getTileSize().height; // tile y coord	
	return cocos2d::Vec2(x, y); // return tile coords
}