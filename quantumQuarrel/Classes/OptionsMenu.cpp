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

#include "OptionsMenu.h"
#include <math.h>

USING_NS_CC;
Scene* OptionsMenu::createScene() {
	auto scene = Scene::create();
	auto layer = OptionsMenu::create();
	scene->addChild(layer);
	return scene;

}
bool OptionsMenu::init() {
	if (!Scene::init()) {
		return false;
	}

	//Initializing all of the sprites and background
	deadZone = 0.1;
	cursorPosition = 1;
	previousCursorPosition = cursorPosition;
	optionsMenuItemSelected = false;

	//new for button cooldown
	bool prevbutton = false;
	bool previbuttonPressed = prevbutton;
	buttonCooldown = previbuttonPressed;
	//end button cooldown

	auto winSize = Director::getInstance()->getVisibleSize();
	int scaleXFactor = 2;
	int scaleYFactor = 2;
	Vec2 orgin = Director::getInstance()->getVisibleOrigin();

	OptionMenuBackground = Sprite::create("optionsMenuAssets/option_menu_background.png");
	OptionMenuBackground->setPosition(winSize.width / 2, winSize.height);
	//Need to scale the image to fit the screen
	OptionMenuBackground->setScaleX((winSize.width / OptionMenuBackground->getContentSize().width) * scaleXFactor);
	OptionMenuBackground->setScaleY((winSize.height / OptionMenuBackground->getContentSize().height) * scaleYFactor);
	OptionMenuBackground->setVisible(true);
	this->addChild(OptionMenuBackground);

	//audio title
	TitleAudio = Sprite::create("optionsMenuAssets/audio_title.png");
	TitleAudio->setPosition(winSize.width / 2, winSize.height / 1.1);
	TitleAudio->setVisible(true);
	this->addChild(TitleAudio);

	//sound title
	SoundEffectsTitle = Sprite::create("optionsMenuAssets/sound_effects_title.png");
	SoundEffectsTitle->setPosition(winSize.width / 2, winSize.height / 1.3); //0.7
	SoundEffectsTitle->setVisible(true);
	this->addChild(SoundEffectsTitle);


	//sound bar
	OneSoundBar = Sprite::create("optionsMenuAssets/sound_bar_1.png");
	OneSoundBar->setPosition(winSize.width / 2, winSize.height / 1.6);
	OneSoundBar->setVisible(true);
	this->addChild(OneSoundBar);

	TwoSoundBar = Sprite::create("optionsMenuAssets/sound_bar_2.png");
	TwoSoundBar->setPosition(winSize.width / 2, winSize.height / 1.6);
	TwoSoundBar->setVisible(false);
	this->addChild(TwoSoundBar);

	ThreeSoundBar = Sprite::create("optionsMenuAssets/sound_bar_3.png");
	ThreeSoundBar->setPosition(winSize.width / 2, winSize.height / 1.6);
	ThreeSoundBar->setVisible(false);
	this->addChild(ThreeSoundBar);

	FourSoundBar = Sprite::create("optionsMenuAssets/sound_bar_4.png");
	FourSoundBar->setPosition(winSize.width / 2, winSize.height / 1.6);
	FourSoundBar->setVisible(false);
	this->addChild(FourSoundBar);

	FiveSoundBar = Sprite::create("optionsMenuAssets/sound_bar_5.png");
	FiveSoundBar->setPosition(winSize.width / 2, winSize.height / 1.6);
	FiveSoundBar->setVisible(false);
	this->addChild(FiveSoundBar);

	SixSoundBar = Sprite::create("optionsMenuAssets/sound_bar_6.png");
	SixSoundBar->setPosition(winSize.width / 2, winSize.height / 1.6);
	SixSoundBar->setVisible(false);
	this->addChild(SixSoundBar);

	SevenSoundBar = Sprite::create("optionsMenuAssets/sound_bar_7.png");
	SevenSoundBar->setPosition(winSize.width / 2, winSize.height / 1.6);
	SevenSoundBar->setVisible(false);
	this->addChild(SevenSoundBar);
	//end sound bar

	//start arrows for sound
	SoundArrowLeftOne = Sprite::create("optionsMenuAssets/sound_arrows_left_1.png");
	SoundArrowLeftOne->setPosition(winSize.width / 2.6, winSize.height * (0.4));
	SoundArrowLeftOne->setVisible(true);
	this->addChild(SoundArrowLeftOne);

	SoundArrowLeftTwo = Sprite::create("optionsMenuAssets/sound_arrows_left_2.png");
	SoundArrowLeftTwo->setPosition(winSize.width / 2.6, winSize.height * (0.4));
	SoundArrowLeftTwo->setVisible(false);
	this->addChild(SoundArrowLeftTwo);

	SoundArrowRightOne = Sprite::create("optionsMenuAssets/sound_arrows_right_2.png");
	SoundArrowRightOne->setPosition(winSize.width / 1.6, winSize.height * (0.4));
	SoundArrowRightOne->setVisible(true);
	this->addChild(SoundArrowRightOne);

	SoundArrowRightTwo = Sprite::create("optionsMenuAssets/sound_arrows_right_1.png");
	SoundArrowRightTwo->setPosition(winSize.width / 1.6, winSize.height * (0.4));
	SoundArrowRightTwo->setVisible(false);
	this->addChild(SoundArrowRightTwo);


	//back
	OneBackButton = Sprite::create("optionsMenuAssets/back_button_1.png");
	OneBackButton->setPosition(winSize.width / 2, winSize.height * (0.2));
	OneBackButton->setVisible(true);
	this->addChild(OneBackButton);

	TwoBackButton = Sprite::create("optionsMenuAssets/back_button_2.png");
	TwoBackButton->setPosition(winSize.width / 2, winSize.height * (0.2));
	TwoBackButton->setVisible(false);
	this->addChild(TwoBackButton);

	this->scheduleUpdate();
	schedule(CC_SCHEDULE_SELECTOR(OptionsMenu::getOptionsControllerInput), 0.2f, kRepeatForever, 0);


	return true;

}
void OptionsMenu::update(float dt) {
	updateOptionsMenu();
	if (optionsMenuItemSelected) {
		exitOptionsMenu();
	}
}

void OptionsMenu::exitOptionsMenu() {
	//volume was here
	//menu
	if (cursorPosition == 3) { //was 2
		//Load main Menu
		bool prevbutton = false;
		auto mainMenu = MainMenu::createScene(prevbutton, volumeValue);
		Director::getInstance()->replaceScene(mainMenu);
	}
}



void OptionsMenu::getOptionsControllerInput(float dt) {
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
		if (axes[1] < deadZone * -1) {
			cursorPosition -= 1;
		}
		if (buttons[0] == 1) {
			optionsMenuItemSelected = true;
		}
		if (buttons[0] == 0) {
			optionsMenuItemSelected = false;
			buttonCooldown = false;
		}
	}
}


void OptionsMenu::updateOptionsMenu() {
	//Allows looping between menu Items
	if (cursorPosition < 1) {
		cursorPosition = NUM_OF_OPTION_MENU_ITEMS;
	}
	if (cursorPosition > NUM_OF_OPTION_MENU_ITEMS) {
		cursorPosition = 1;
	}
	if (previousCursorPosition != cursorPosition) {
		cursorMoved = true;
		previousCursorPosition = cursorPosition;
	}

	if (cursorMoved) {
		if (!buttonCooldown) {
			//sound left arrow
			if (cursorPosition == 1) {
				SoundArrowLeftTwo->setVisible(true);
				SoundArrowLeftOne->setVisible(false);
				volumeCount -= 1;
				buttonCooldown = true;
				CCLOG("Volume Count", volumeCount);
			}
			else {
				SoundArrowLeftTwo->setVisible(false);
				SoundArrowLeftOne->setVisible(true);
			}
			//sound right arrow
			if (cursorPosition == 2) {
				SoundArrowRightTwo->setVisible(true);
				SoundArrowRightOne->setVisible(false);
				volumeCount += 1;
				buttonCooldown = true;
				CCLOG("Volume Count", volumeCount);
			}
			else {
				SoundArrowRightTwo->setVisible(false);
				SoundArrowRightOne->setVisible(true);
			}
			//back button
			if (cursorPosition == 3) {
				TwoBackButton->setVisible(true);
				OneBackButton->setVisible(false);
			}
			else {
				TwoBackButton->setVisible(false);
				OneBackButton->setVisible(true);
			}
		}

		//wrap around
		if (volumeCount == 0) {
			volumeCount = 7;
		}
		if (volumeCount == 8) {
			volumeCount = 1;
		}

		if (volumeCount == 1) {
			OneSoundBar->setVisible(true);
			SevenSoundBar->setVisible(false);
			TwoSoundBar->setVisible(false);
			volumeValue = 0.1f;
		}
		if (volumeCount == 2) {
			TwoSoundBar->setVisible(true);
			OneSoundBar->setVisible(false);
			ThreeSoundBar->setVisible(false);
			volumeValue = 0.2f;
		}
		if (volumeCount == 3) {
			ThreeSoundBar->setVisible(true);
			TwoSoundBar->setVisible(false);
			FourSoundBar->setVisible(false);
			volumeValue = 0.3f;
		}

		if (volumeCount == 4) {
			FourSoundBar->setVisible(true);
			ThreeSoundBar->setVisible(false);
			FiveSoundBar->setVisible(false);
			volumeValue = 0.4f;
		}
		if (volumeCount == 5) {
			FiveSoundBar->setVisible(true);
			FourSoundBar->setVisible(false);
			SixSoundBar->setVisible(false);
			volumeValue = 0.5f;
		}
		if (volumeCount == 6) {
			SixSoundBar->setVisible(true);
			FiveSoundBar->setVisible(false);
			SevenSoundBar->setVisible(false);
			volumeValue = 0.6f;
		}
		if (volumeCount == 7) {
			SevenSoundBar->setVisible(true);
			SixSoundBar->setVisible(false);
			OneSoundBar->setVisible(false);
			volumeValue = 0.7f;
		}

	}

}

/*
Implement Controller input into the menu so the user can scroll / select different icons.
(Pressing the(A / X / B) (Nintendo Switch, Playstation, Xbox)) should call on a function(can leave it blank for now)
that will allow us to load a new scene(functions to load new scenes don't have to be implemented yet)
*/