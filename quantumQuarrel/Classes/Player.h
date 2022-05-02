#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include "fmod.hpp"
#include "AudioManager.h"
#pragma once
using namespace cocos2d;

class Player
{
public:
	//CREATE_FUNC(Player);

	virtual bool init(int gravStrength, TMXTiledMap* initMap, Rect boundingBox, int playerNumber);
	void update(float);
	void spawn(Vec2 pos);
	void jump();
	void run(float vel);
	bool InAir(Vec2 position);
	bool hitDeathPlane(Vec2 currentPosition);
	void loadAnimations();
	void updateStunStatus();
	bool isStuned;
	bool isAttacking;
	bool Attacked();
	void setHitBox(Rect);
	void setAttackBox(Rect);
	void setFootPos(Vec2);
	void reset();
	bool ComboChain(float dtF, float dtI);
	int numTimesAttacked;
	float comboStartTime;
	bool beginComboChain;
	bool endChain;
	Vector<SpriteFrame*> JumpAnimation;
	Animate* jumpAnimate;
	int NUM_JUMP_FRAMES;
	Vector<SpriteFrame*> walkAnimation;
	Animate* walkAnimate;
	int NUM_WALK_FRAMES;
	Vector<SpriteFrame*> idleAnimation;
	Animate* idleAnimate;
	int NUM_IDLE_FRAMES;
	int NUM_ATTACK_FRAMES;
	Vector<SpriteFrame*> attackAnimation;
	Animate* attackAnimate;
	Vec2 tileCoordForPosition(Vec2 position); // Converting pixel values to x,y coordinates on the tilemap
	Rect boundingBox;
	TMXLayer* _background;
	TMXLayer* _ground;
	TMXLayer* _DeathPlane;
	TMXObjectGroup* objectGroup;
	Rect hitBox;
	Rect attackBox;
	Action* walking; // tag =1
	Action* jumping;// tag = 3
	Action* idling;// tag = 0
	Action* attacking; //tag =2
	float damage;
	float comboCooldown;
	float comboCooldownTime;
	bool onCooldown;
	float lastAttackTime;
    

	TMXTiledMap* _CurMap;
	Sprite *playerSprite;
	Vec2 position;
	Vec2 footPos;
	Vec2 velocity;
	Vec2 acceleration;
	Vec2 gravity;
	Vec2 Spawnpoint;
	int playerNumber;
	int playerLives;
	int orientation; // Direction the player is facing: 1 is right, -1 is left
	bool canJump;
	bool onGround;
	bool attackButtonPressed;
	float maxVel = 500;
	float radius;
	float height;
	float width;
	FMOD::System *system;
	FMOD::Sound *background_sound;
	FMOD::Sound *death_sound;
	FMOD::Sound *jump_sound;
	FMOD::Sound *attack_sound;
	FMOD::Channel *background_channel;
	FMOD::Channel *death_channel;
	FMOD::Channel *attack_channel;
	FMOD::Channel *jump_channel;
	AudioManager sound_vol;
};
#endif // __PLAYER_H__
