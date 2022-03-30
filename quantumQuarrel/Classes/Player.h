#include "cocos2d.h"
using namespace cocos2d;

class Player
{
public:
	//CREATE_FUNC(Player);

	virtual bool init(int, TMXTiledMap*, Rect);
	void update(float);
	void spawn(Vec2);
	void jump();
	void run(float);
	bool InAir(Vec2 position);
	bool hitDeathPlane(Vec2 currentPosition);
	Vec2 tileCoordForPosition(Vec2 position); // Converting pixel values to x,y coordinates on the tilemap
	Rect boundingBox;
	TMXLayer* _background;
	TMXLayer* _ground;
	TMXLayer* _DeathPlane;
	TMXObjectGroup* objectGroup;

	TMXTiledMap* _CurMap;
	Sprite *playerSprite;
	Vec2 position;
	Vec2 velocity;
	Vec2 acceleration;
	Vec2 gravity;
	Vec2 SpawnpointP1;

	bool canJump;
	bool onGround;
	float maxVel = 500;
	float radius;


};