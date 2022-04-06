#include "cocos2d.h"
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
	void updateStunStatus();
	bool isStuned;
	bool isAttacking;
	bool Attacked();
	void setHitBox(Rect);
	Vec2 tileCoordForPosition(Vec2 position); // Converting pixel values to x,y coordinates on the tilemap
	Rect boundingBox;
	TMXLayer* _background;
	TMXLayer* _ground;
	TMXLayer* _DeathPlane;
	TMXObjectGroup* objectGroup;
	Rect hitBox;

	TMXTiledMap* _CurMap;
	Sprite *playerSprite;
	Vec2 position;
	Vec2 velocity;
	Vec2 acceleration;
	Vec2 gravity;
	Vec2 Spawnpoint;
	int playerNumber;
	int playerLives;
	int orientation; // Direction the player is facing: 1 is right, -1 is left
	bool canJump;
	bool onGround;
	float maxVel = 500;
	float radius;


};