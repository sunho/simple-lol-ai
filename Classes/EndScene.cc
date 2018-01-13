//
//  EndScene.cpp
//  AI
//
//  Created by sunho kim on 2017/12/20.
//

#include "EndScene.hpp"
using namespace cocos2d;
bool EndScene::init(std::string con1,std::string con2,int winlost)
{
    Scene::init();
    this->addChild(LayerColor::create(ccc4(255, 255, 255, 255)));
    cocos2d::Sprite* con1s;
    cocos2d::Sprite* con2s;
    if(con1=="trad")
    {
        con1s=Sprite::create("oldai.png");
    }
    else if(con1=="deep")
    {
        con1s=Sprite::create("newai.png");
    }
    else
    {
        con1s=Sprite::create("human.png");
    }
    if(con2=="trad")
    {
        con2s=Sprite::create("oldai.png");
    }
    else if(con2=="deep")
    {
        con2s=Sprite::create("newai.png");
    }
    else
    {
        con2s=Sprite::create("human.png");
    }
    con1s->setPosition(Vec2(523,1080-550));
    con2s->setPosition(Vec2(1395,1080-550));
    if(winlost==1)
    {
        con1s->setScale(1.1);
        con2s->setScale(0.9);
    }
    else if(winlost==-1)
    {
        con2s->setScale(1.1);
        con1s->setScale(0.9);
    }
    this->addChild(con1s);
    this->addChild(con2s);
    auto vs = Label::createWithTTF(Comander::getInstance()->interesting(),"fonts/abc.otf", 20);
    vs->setPosition(960, 200);
    vs->setAnchorPoint(Vec2(0.5,0.5));
    vs->setColor(ccc3(0,0,0));
    this->addChild(vs);
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        return true;
    };
    listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        auto scene=MainScene::create();
        cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(0.5,scene, cocos2d::Color3B(0,0,0)));
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

EndScene* EndScene::create(std::string con1,std::string con2,int winlost)
{
    EndScene *pRet=new(std::nothrow) EndScene();
    if(pRet && pRet->init(con1,con2,winlost))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet=nullptr;
        return nullptr;
    }
}
