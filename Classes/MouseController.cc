//
//  MouseController.cpp
//  AI
//
//  Created by sunho kim on 2017/11/09.
//

#include "MouseController.hpp"
#define PI 3.14159265
using namespace cocos2d;
bool MouseController::init()
{
    Sprite::init();
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        cocos2d::Vec2 p = touch->getLocation();
        float xline=p.x-_player->getX();
        float yline=p.y-_player->getY();
        float dis=sqrt(pow(xline,2)+pow(yline,2));
        float radi;
        if(xline>0)
        {
            radi=atan(yline/xline)*(180/PI);
        }
        else if(xline<0)
        {
            radi=atan(yline/xline)*(180/PI)+180;
        }
        _player->move(_player->getDirection(radi),dis);
        return true;
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    auto mouse = EventListenerMouse::create();
    mouse->onMouseMove  = [&](cocos2d::Event* event)
    {
        auto mouseEvent = static_cast<EventMouse*>(event);
        _mouseX=mouseEvent->getCursorX();
        _mouseY=mouseEvent->getCursorY();
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouse, this);
    auto keylistener = EventListenerKeyboard::create();
    keylistener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event)
    {
        if(keyCode==EventKeyboard::KeyCode::KEY_Q)
        {
            float xline=_mouseX-_player->getX();
            float yline=_mouseY-_player->getY();
            float dis=sqrt(pow(xline,2)+pow(yline,2));
            float radi;
            if(xline>0)
            {
                radi=atan(yline/xline)*(180/PI);
            }
            else if(xline<0)
            {
                radi=atan(yline/xline)*(180/PI)+180;
            }
            _player->q(_player->getDirection(radi));
        }
        if(keyCode==EventKeyboard::KeyCode::KEY_W)
        {
            _player->w();
        }
        if(keyCode==EventKeyboard::KeyCode::KEY_E)
        {
            float xline=_mouseX-_player->getX();
            float yline=_mouseY-_player->getY();
            float dis=sqrt(pow(xline,2)+pow(yline,2));
            float radi;
            if(xline>0)
            {
                radi=atan(yline/xline)*(180/PI);
            }
            else if(xline<0)
            {
                radi=atan(yline/xline)*(180/PI)+180;
            }
            _player->e(_player->getDirection(radi),dis);
        }
        if(keyCode==EventKeyboard::KeyCode::KEY_A)
        {
            _player->attack();
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keylistener, this);
    return true;
}

void MouseController::updates(int timestep)
{
}
