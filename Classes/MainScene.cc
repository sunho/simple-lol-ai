//
//  MainScene.cpp
//  AI
//
//  Created by sunho kim on 04/11/2017.
//

#include "MainScene.hpp"
#include "PlayerSelectorGroup.hpp"
#include "GameScene.hpp"
#include "MouseController.hpp"
#include "OldController.hpp"
#include "NewController.hpp"
#include "KeyboardController.hpp"
#include "Commander.hpp"
USING_NS_CC;



// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}
std::string ptos(PlayerSelector::Player player)
{
    if(player==PlayerSelector::NewAI)
    {
        return "deep";
    }
    if(player==PlayerSelector::OldAI)
    {
        return "trad";
    }
    if(player==PlayerSelector::Human)
    {
        return "human";
    }
}
// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    this->addChild(LayerColor::create(ccc4(255, 255, 255, 255)));
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto player1 = Label::createWithTTF("Player 1", "fonts/abc.otf", 96);
    player1->setPosition(260, visibleSize.height-60);
    player1->setAnchorPoint(Vec2(0,1));
    player1->setColor(ccc3(0,0,0));
    this->addChild(player1);
    auto vs = Label::createWithTTF("VS", "fonts/abc.otf", 80);
    vs->setPosition(960, 540);
    vs->setAnchorPoint(Vec2(0.5,0.5));
    vs->setColor(ccc3(0,0,0));
    this->addChild(vs);
    auto player2 = Label::createWithTTF("Player 2", "fonts/abc.otf", 96);
    player2->setPosition(visibleSize.width-260, visibleSize.height-60);
    player2->setAnchorPoint(Vec2(1,1));
    player2->setColor(ccc3(0,0,0));
    this->addChild(player2);
    this->gotone=false;
    auto group = PlayerSelectorGroup::create();
    this->addChild(group);
    
    auto playButton = Sprite::create("play.png");
    playButton->setPosition(1653.5,visibleSize.height-932.95);
    this->addChild(playButton);
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        return true;
    };
    listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        cocos2d::Vec2 p = touch->getLocation();
        cocos2d::Rect rect = playButton->getBoundingBox();
        if(rect.containsPoint(p))
        {
            if(this->gotone==false)
            {
                std::string p1=ptos(group->player1()->getSelected());
                std::string p2=ptos(group->player2()->getSelected());
                if(p1=="human"&&p2=="human")
                {
                    
                }
                else
                {
                    Comander::getInstance()->interpret("viewgame,"+p1+","+p2);
                     this->gotone=true;
                }
            }
        }
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}


