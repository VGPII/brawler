#pragma once
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

#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "cocos2d.h"
#include "BallBounceScene.h"
#include "fmod.hpp"
#include "AudioManager.h"
#include "OptionsMenu.h"

using namespace cocos2d;

class MainMenu : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene(bool prevbutton, float volume = 0.5f);

	virtual bool init(float volume = 0.5f);
	void update(float dt);
	void updateMenu();
	void getControllerInput(float dt);
	void ExitMainMenu();
	bool menuItemSelected;
	bool buttonCooldown;
	bool controllerPresence;
	const float* axes;
	int axesCount;
	const unsigned char* buttons;
	int buttonCount;
	float deadZone;
	CREATE_FUNC(MainMenu);
	Sprite* MenuBackground;
	Sprite* Start1;
	Sprite* Start2;
	Sprite* Options1;
	Sprite* Options2;
	Sprite* TitleIcon;
	int cursorPosition;
	int NUM_OF_MENU_ITEMS = 2;
	int previousCursorPosition;
	bool cursorMoved;

	FMOD::System *system;
	FMOD::Sound *background_sound;
	FMOD::Sound *intro_sound;
	FMOD::Channel *intro_channel;
	AudioManager sound_vol;
	//OptionsMenu volume_level;
};

#endif // __MAINMENU__H__
