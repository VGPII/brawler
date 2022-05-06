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

#include "PostGameScene.h"
#include <math.h>
#include "fmod.hpp"
#include "AudioManager.h"
USING_NS_CC;
int playerText;
float postGameVolume;
Scene* PostGameScene::createScene(int playerWon, float volume) {
	postGameVolume = volume;
	playerText = playerWon;
	auto scene = Scene::create();
	auto layer = PostGameScene::create();
	scene->addChild(layer);
	return scene;

}
bool PostGameScene::init() {
	if (!Scene::init()) {
		return false;
	}
	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, nullptr);
	sound_vol.setVolume(postGameVolume);
	winSize = cocos2d::Director::getInstance()->getVisibleSize();
	if (playerText == 1) {
		auto label1 = Label::createWithSystemFont("Player 1 wins", "Arial", 96);
		label1->setAnchorPoint(cocos2d::Vec2(0,0));
		label1->setPosition(cocos2d::Vec2(winSize.width / 4, winSize.height / 2));
		this->addChild(label1, 1);
		//file path
		std::string p1WinPath = FileUtils::getInstance()->fullPathForFilename("audio/p1_win.wav");
		//create sound
		system->createSound(p1WinPath.c_str(), FMOD_LOOP_OFF, 0, &p1WinSound);
		//init sound but set to pause until volume set
		system->playSound(p1WinSound, 0, true, &p1Win);
		//set volume (background music needs to be quieter, so it gets a .1f multiplier
		p1Win->setVolume(sound_vol.getVolume());
		//play background music on loop
		p1Win->setPaused(false);
		//file path
		std::string applausePath = FileUtils::getInstance()->fullPathForFilename("audio/applause.wav");
		//create sound
		system->createSound(applausePath.c_str(), FMOD_LOOP_OFF, 0, &applause_sound);
		//init
		system->playSound(applause_sound, 0, true, &applause);
		//set volume
		applause->setVolume(sound_vol.getVolume());
		//play applause
		applause->setPaused(false);

	}
	else {
		CCLOG("Player 2 Wins");
		auto label1 = Label::createWithSystemFont("Player 2 wins", "Arial", 96);
		label1->setAnchorPoint(cocos2d::Vec2(0, 0));
		label1->setPosition(cocos2d::Vec2(winSize.width / 4, winSize.height / 2));
		this->addChild(label1, 1);
		//file path
		std::string p2WinPath = FileUtils::getInstance()->fullPathForFilename("audio/p2_win.wav");
		//create sound
		system->createSound(p2WinPath.c_str(), FMOD_LOOP_OFF, 0, &p2WinSound);
		//init sound but set to pause until volume set
		system->playSound(p2WinSound, 0, true, &p2Win);
		//set volume (background music needs to be quieter, so it gets a .1f multiplier
		p2Win->setVolume(sound_vol.getVolume());
		//play background music on loop
		p2Win->setPaused(false);
		//file path
		std::string applausePath = FileUtils::getInstance()->fullPathForFilename("audio/applause.wav");
		//create sound
		system->createSound(applausePath.c_str(), FMOD_LOOP_OFF, 0, &applause_sound);
		//init
		system->playSound(applause_sound, 0, true, &applause);
		//set volume
		applause->setVolume(sound_vol.getVolume());
		//play applause
		applause->setPaused(false);

	}
	auto label2 = Label::createWithSystemFont("Press A/B to return to the Main Menu", "Arial", 50);
	label2->setAnchorPoint(cocos2d::Vec2(0,0));
	label2->setPosition(cocos2d::Vec2(winSize.width / 8, winSize.height / 4));
	this->addChild(label2, 1);
	scheduleUpdate();
	return true; 
}
void PostGameScene::update(float dt) {
	getControllerInput(dt);
	if (menuItemSelected) {
		ExitScene();
	}

}

void PostGameScene::ExitScene() {
	bool buttonPressed = true;
	auto mainMenu= MainMenu::createScene(buttonPressed);
	Director::getInstance()->replaceScene(mainMenu);
}
void PostGameScene::getControllerInput(float dt) {
    controllerPresence = glfwJoystickPresent(GLFW_JOYSTICK_1); // Player 1 Only
   //CCLOG("Controller Presence %d", controllerPresence);
    if (controllerPresence >= 1) {
        axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
      /*  CCLOG("\n\n\n\n\n");
        CCLOG("Number of Axes: %d", axesCount);// Most controllers have 4 axis (x,y for each stick), but some have 6 (analog triggers)
       // Main problem is dead zone (when the stick is in the center the function does not return exactly 0)
        //CCLOG("Left Stick X Axis: %f", axes[0]); // Returns a value between -1 and 1
        CCLOG("Left Stick Y Axis: %f", axes[1]);
        CCLOG("Right Stick X Axis: %f", axes[2]);
        CCLOG("Right Stick Y Axis : %f", axes[3]); // For Ps4 this is L2 (Trigger)
        CCLOG("Left Trigger/L2: %f", axes[4]);
        CCLOG("Right Trigger/R2: %f", axes[5]); // For Ps4 controller this is the right stick y-axis
       */
        if (buttons[0] == 1) {
            menuItemSelected = true;
        }
    }
}