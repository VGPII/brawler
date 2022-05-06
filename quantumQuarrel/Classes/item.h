#include "cocos2d.h"
#ifndef __ITEM_H__	
#define __ITEM__
#pragma once

using namespace cocos2d;

class Item
{
public:
	//CREATE_FUNC(Item);
	// FUNCTIONS
	virtual bool init(int gravStrength, int initType, TMXTiledMap* initMap);
	void update(float dt);
	void pickUp();
	void setBasePos(Vec2 newPos);
	bool inAir(Vec2 Currentposition);
	Vec2 tileCoordForPosition(Vec2 CurrentPosition);
	// DATA
	Sprite* itemSprite;
	TMXTiledMap* _CurMap;
	TMXLayer* _ground;
	TMXObjectGroup* objectGroup;
	int type;
	float idleDuration; // Durations in seconds
	float buffDuration;
	bool isEquiped;
	Rect hitBox;
	Vec2 position;
	Vec2 basePos;
	Vec2 velocity;
	Vec2 acceleration;
	Vec2 gravity;
	Vec2 Spawnpoint;
	bool setToDespawn;
	float height;
	float width;
};
#endif // __ITEM_H__



