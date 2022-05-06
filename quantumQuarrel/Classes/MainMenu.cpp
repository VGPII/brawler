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

#include "MainMenu.h"
#include <math.h>
#include "fmod.hpp"	
#include "AudioManager.h"	
#include "OptionsMenu.h"	
#include "BallBounceScene.h"


USING_NS_CC;

bool previbuttonPressed;
float volume_per;
float volume1;
int menuCount;
bool gameinit = true;
FMOD::Channel* background_channel;
Scene* MainMenu::createScene(bool prevbutton=false, float volume) {
    previbuttonPressed = prevbutton;
    volume_per = volume;
    previbuttonPressed = prevbutton;
	auto scene = Scene::create();
	auto layer = MainMenu::create();
	scene->addChild(layer);
	return scene;

}
bool MainMenu::init() {
	if (!Scene::init()) {
		return false;
	}
    buttonCooldown = previbuttonPressed;
    //Initializing all of the sprites and background	
    deadZone = 0.1;
    cursorPosition = 1;
    previousCursorPosition = cursorPosition;
    menuItemSelected = false;
    buttonCooldown = previbuttonPressed;
    //Initializing all of the sprites and background
    deadZone = 0.1;
    cursorPosition = 1;
    previousCursorPosition = cursorPosition;
    menuItemSelected = false;
	auto winSize = Director::getInstance()->getVisibleSize();
	int scaleXFactor = 2;
	int scaleYFactor = 2;
	Vec2 orgin = Director::getInstance()->getVisibleOrigin();
	MenuBackground = Sprite::create("menuAssets/background_menu.png");
	MenuBackground->setPosition(winSize.width / 2, winSize.height);
    //Need to scale the image to fit the screen
	MenuBackground->setScaleX((winSize.width / MenuBackground->getContentSize().width) * scaleXFactor);
	MenuBackground->setScaleY((winSize.height / MenuBackground->getContentSize().height) * scaleYFactor);
	MenuBackground->setVisible(true);
	this->addChild(MenuBackground);

	TitleIcon = Sprite::create("menuAssets/title_icon.png");
	TitleIcon->setPosition(winSize.width/2, winSize.height/1.1);
	TitleIcon->setVisible(true);
	this->addChild(TitleIcon);

	Start1 = Sprite::create("menuAssets/start_1.png");
	Start1->setPosition(winSize.width / 2, winSize.height*(0.7));
	Start1->setVisible(false);
	this->addChild(Start1);

	Start2 = Sprite::create("menuAssets/start_2.png");
	Start2->setPosition(winSize.width / 2, winSize.height*(0.7));
	Start2->setVisible(true);
	this->addChild(Start2);

	Options1 = Sprite::create("menuAssets/options_1.png");
	Options1->setPosition(winSize.width / 2, winSize.height*(0.4));
	Options1->setVisible(true);
	this->addChild(Options1);

	Options2 = Sprite::create("menuAssets/options_2.png");
	Options2->setPosition(winSize.width / 2, winSize.height* (0.4));
	Options2->setVisible(false);
	this->addChild(Options2);

    this->scheduleUpdate();

    //sound is hard-coded to 0.5f, but options menu will return a float based on user selection.  That value will be used in place of 0.5f	
    //create and initialize fmod audio system	
    if (gameinit == false) {
        volume1 = volume_per;
    }
    if (gameinit == true) {
        volume1 = 0.5f;
        gameinit = false;
    }
    sound_vol.setVolume(volume1);
    FMOD::System_Create(&system);
    system->init(32, FMOD_INIT_NORMAL, nullptr);
    if (menuCount < 1) {
        //file path	
        std::string background_path = FileUtils::getInstance()->fullPathForFilename("audio/background.wav");
        //create sound	
        system->createSound(background_path.c_str(), FMOD_LOOP_NORMAL, 0, &background_sound);
        //init sound but set to pause until volume set	
        system->playSound(background_sound, 0, true, &background_channel);
        //set volume (background music needs to be quieter, so it gets a .1f multiplier	
        //play background music on loop	
        background_channel->setVolume(volume1 * 0.1f);
        background_channel->setPaused(false);
        menuCount = menuCount + 1;
    }
    background_channel->setPaused(true);
    background_channel->setVolume(volume1 * 0.1f);
    background_channel->setPaused(false);
    schedule(CC_SCHEDULE_SELECTOR(MainMenu::getControllerInput), 0.1f, kRepeatForever, 0); // get controller input every 0.1 seconds	
    return true;
	
}
void MainMenu::update(float dt) {
    updateMenu();
    if (menuItemSelected) {
        ExitMainMenu();
    }
}
void MainMenu::ExitMainMenu() {
    if (!buttonCooldown) {
        if (cursorPosition == 1) {
            //Load main Game	
            //file path	
            std::string intro_path = FileUtils::getInstance()->fullPathForFilename("audio/intro.wav");
            //create sound	
            system->createSound(intro_path.c_str(), FMOD_LOOP_OFF, 0, &intro_sound);
            //sound is initialized but set to paused until volume is set	
            system->playSound(intro_sound, 0, true, &intro_channel);
            //set volume	
            intro_channel->setVolume(sound_vol.getVolume());
            //play (unpause) sound	
            intro_channel->setPaused(false);
            auto mainGame = BallBounce::createScene(volume_per);
            Director::getInstance()->replaceScene(mainGame);
        }
        if (cursorPosition == 2) {
            //Load Options Menu 	
            auto menuScreen = OptionsMenu::createScene();
            Director::getInstance()->replaceScene(menuScreen);
        }
    }
}
void MainMenu::getControllerInput(float dt) {
    controllerPresence = glfwJoystickPresent(GLFW_JOYSTICK_1); // Player 1 Only
    //CCLOG("Controller Presence %d", controllerPresence);

    if (controllerPresence >= 1) {
        axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        CCLOG("\n\n\n\n\n");
        CCLOG("Number of Axes: %d", axesCount);// Most controllers have 4 axis (x,y for each stick), but some have 6 (analog triggers)
       // Main problem is dead zone (when the stick is in the center the function does not return exactly 0)
        //CCLOG("Left Stick X Axis: %f", axes[0]); // Returns a value between -1 and 1
        CCLOG("Left Stick Y Axis: %f", axes[1]);
        CCLOG("Right Stick X Axis: %f", axes[2]);
        CCLOG("Right Stick Y Axis : %f", axes[3]); // For Ps4 this is L2 (Trigger)
        CCLOG("Left Trigger/L2: %f", axes[4]);
        CCLOG("Right Trigger/R2: %f", axes[5]); // For Ps4 controller this is the right stick y-axis
        if (axes[1] > deadZone) {
            cursorPosition += 1;
        }
        if (axes[1] < deadZone*-1) {
            cursorPosition -= 1;
        }
        if (buttons[0] == 1) {
            menuItemSelected = true;
        }
        if (buttons[0] == 0) {
            menuItemSelected = false;
            buttonCooldown = false;
        }
    }
}

void MainMenu::updateMenu() {
    //Allows looping between menu Items
    if (cursorPosition < 1) {
        cursorPosition = NUM_OF_MENU_ITEMS;
    }
    if (cursorPosition > NUM_OF_MENU_ITEMS) {
        cursorPosition = 1;
    }
    if (previousCursorPosition != cursorPosition) {
        cursorMoved = true;
        previousCursorPosition = cursorPosition;
    }
    if (cursorMoved) {
        if (cursorPosition == 1) {
            Start2->setVisible(true); 
            Start1->setVisible(false);
        }
        else {
            Start2->setVisible(false);
            Start1->setVisible(true);
        }
        if (cursorPosition == 2) {
            Options2->setVisible(true);
            Options1->setVisible(false);
        }
        else {
            Options1->setVisible(true);
            Options2->setVisible(false);
        }

    }

}
