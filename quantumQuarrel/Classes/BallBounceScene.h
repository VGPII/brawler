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

#ifndef __BALLBOUNCE_SCENE_H__
#define __BALLBOUNCE_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "MainMenu.h"
#include "PostGameScene.h"
using namespace cocos2d;
class BallBounce : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(BallBounce);

	void update(float) override;
	void setViewPointCenter(Vec2 position); // Centering the camera around the ball sprite
	
	bool checkForCollision(Rect Attacker, Rect Reciver); // Checks for collision between the attacker's hit box, and the other player's bounding box
	void calculateKnockback(Player* Reciver, Player* Attacker); // Caclulates the knockback of the reciver based on the orientation of the attacker
	void toMainMenu();
	void toPostGameScene(int playerWon);
	void playerWon();
	

	int gravity;

	TMXTiledMap* _MainMap;
	TMXLayer* _background;
	TMXLayer* _ground;
	TMXLayer* _DeathPlane;
	ValueMap* playerSpawnPoint;
	DrawNode *node;
	Size winSize;

	Player* playerOne;
	Player* playerTwo;

	bool canJump;
	bool onGround;
	float maxVel = 500;
	float radius;
	float dtF; // Current Time
	float dtI; // Previous Time

	bool debugMode = false;
	void drawBox(DrawNode* node, Vec2 bottomLeft, Vec2 topRight);
	void drawBox(DrawNode* node, Rect rectangle, Color4F color);
};



#endif // __BALLBOUNCE_SCENE_H__
