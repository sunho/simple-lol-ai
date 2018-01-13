//
//  KeyboardController.cpp
//  AI
//
//  Created by sunho kim on 2017/11/09.
//

#include "KeyboardController.hpp"
#define PI 3.14159265
using namespace cocos2d;
bool KeyboardController::init()
{
    Sprite::init();
    auto keylistener = EventListenerKeyboard::create();
    keylistener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event)
    {
        if(keyCode==EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        {
            _player->move(_player->getDirection(180),5);
        }
        if(keyCode==EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
        {
            _player->move(0,5);
        }
        if(keyCode==EventKeyboard::KeyCode::KEY_DOWN_ARROW)
        {
            _player->move(_player->getDirection(260),5);
        }
        if(keyCode==EventKeyboard::KeyCode::KEY_UP_ARROW)
        {
            _player->move(_player->getDirection(90),5);
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keylistener, this);
    return true;
}

void KeyboardController::updates(int timestep)
{
    
}

