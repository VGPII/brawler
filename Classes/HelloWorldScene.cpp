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

#include "HelloWorldScene.h"
#include "AudioEngine.h"


USING_NS_CC;
float gravity = -9.80f;
float force = 1* gravity;
float velocity1 = 0.0f;
float velocity2 = 0.0f;
float acceleration = force;

Size screenSize = Director::getInstance()->getWinSize();
float positionX1 = 400;

float positionY1 = 200;

float positionX2 = 200;
float positionY2 = 200;
bool keyDown = false;
int keyDirection;
Vec2 sprite_position1;
Vec2 sprite_position2;
bool isBouncingEnabled = true;

float deadZone = 0.1;

bool playAudio = false;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
  
}

bool HelloWorld::init() 
{
    
    
    if (!Layer::init()) {
        return false;
    }
    sprite = Sprite::create("ball_blue.png");
    sprite2 = Sprite::create("ball_red.png");
    sprite->setPosition(positionX1, positionY1);
    sprite2->setPosition(positionX2, positionY2);
    sprite_position1 = sprite->getPosition();
    sprite_position2 = sprite2->getPosition();
    this->addChild(sprite);
    this->addChild(sprite2);
    sprite->setAnchorPoint(Vec2(0, 0));
    this->scheduleUpdate(); // This is needed in order for the update method to activate
    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) {
        Vec2 loc = event->getCurrentTarget()->getPosition();
        switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case EventKeyboard::KeyCode::KEY_A:
            keyDown = true;
            keyDirection = 4;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case EventKeyboard::KeyCode::KEY_D:
            keyDown = true;
            keyDirection = 3;
            break;
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
        case EventKeyboard::KeyCode::KEY_W:
            keyDown = true;
            keyDirection = 1;
            break;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case EventKeyboard::KeyCode::KEY_S:
            keyDown = true;
            keyDirection = 2;
            break;
        }

    };
    eventListener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) {
        Vec2 loc = event->getCurrentTarget()->getPosition();
        switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case EventKeyboard::KeyCode::KEY_A:
            keyDown = false;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case EventKeyboard::KeyCode::KEY_D:
            keyDown = false;
            break;
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
        case EventKeyboard::KeyCode::KEY_W:
            keyDown = false;
            break;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case EventKeyboard::KeyCode::KEY_S:
            keyDown = false;
            break;

        }
    };
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
    return true;
}
void HelloWorld::update(float dt) {
    CCLOG("Screen Size %f", screenSize.width);

    velocity1 += gravity;
    sprite_position1.y += velocity1*0.1;
    if (sprite_position1.y <= 25) {
        if (playAudio) {
            AudioEngine::play2d("bounce.mp3");
        }
        velocity1 *= -1;
        sprite_position1.y = 26.1;
    }
    
   
    
    if (keyDown) {
        switch (keyDirection) {
        case(1):
            sprite_position1.y += 1; 
            break;

        case(2):
            sprite_position1.y -= 1;
            break;
        case(3):
            sprite_position1.x += 1;
            break;
        case(4):
            sprite_position1.x -= 1;
            break;
        }
        
    }
    sprite->setPosition(sprite_position1.x, sprite_position1.y);
    //CCLOG("Hi", 5); // Debug values
   
    int controller_presence = glfwJoystickPresent(GLFW_JOYSTICK_1); 
    //CCLOG("Controller Presence %d", controller_presence);
   // CCLOG(glfwGetVersionString());
    if (controller_presence >= 1) {
        sprite2->setVisible(true);
        int axesCount;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        if (axes[0] > deadZone) {
            sprite_position2.x += 2 * axes[0];

        }
        if (axes[0] < 0) {
            sprite_position2.x += 2 * axes[0];

        }
        if (axes[1] > deadZone) {
            sprite_position2.y -= 2 * axes[1];
        }
        if (axes[1] < 0) {
            sprite_position2.y -= 2 * axes[1];
        }

        velocity2 += gravity;
        sprite_position2.y += velocity2 * 0.1;
        if (sprite_position2.y <= 25) {
            if (playAudio) {
                AudioEngine::play2d("bounce.mp3");
            }
            velocity2 *= -1;
            sprite_position2.y = 26.1;
        }

        //Rumble will take a little bit more effort
        sprite2->setPosition(sprite_position2.x, sprite_position2.y);
        CCLOG("\n\n\n\n\n");
        CCLOG("Number of Axes: %d", axesCount);// Most controllers have 4 axis (x,y for each stick), but some have 6 (analog triggers)
       // Main problem is dead zone (when the stick is in the center the function does not return exactly 0)
        CCLOG("Left Stick X Axis: %f", axes[0]); // Returns a value between -1 and 1
        CCLOG("Left Stick Y Axis: %f", axes[1]);
        CCLOG("Right Stick X Axis: %f", axes[2]);
        CCLOG("Right Stick Y Axis : %f", axes[3]);
       // CCLOG("Left Trigger/L2: %f", axes[4]);
       // CCLOG("Right Trigger/R2: %f", axes[5]);
        int buttonCount;
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        if (GLFW_PRESS == buttons[1]) {
            CCLOG("A button Pressed");
            if (playAudio) {
                playAudio = false;
            }
            else {
                playAudio = true;
            }
            
        }
        if (GLFW_RELEASE == buttons[2]) {
            CCLOG("A button Released");
        }
        const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        CCLOG("The name of the Joystick / gamepad is: %s", name);


    }
    else {
        sprite2->setVisible(false);
    }


}
