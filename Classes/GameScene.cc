//
//  GameScene.cpp
//  AI
//
//  Created by sunho kim on 2017/11/06.
//

#include "GameScene.hpp"
#include "GameConsole.hpp"
#include "PlayerSprite.hpp"
#include "EndScene.hpp"
#include "Commander.hpp"
using namespace cocos2d;
using namespace std;
GameScene* GameScene::create(GameConsole* console)
{
    GameScene *pRet=new(std::nothrow) GameScene();
    if(pRet && pRet->init(console))
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
bool GameScene::init(GameConsole* console)
{
    Scene::init();
    this->addChild(LayerColor::create(ccc4(255, 255, 255, 255)));
    _console = console;
    _console->player1()->addListener(this);
    _console->player2()->addListener(this);
    auto la=Sprite::create();
    auto sprite=PlayerSprite::create(_console->player1());
    auto sprite2=PlayerSprite::create(_console->player2());
    this->schedule(schedule_selector(GameScene::updateConsole), 1.0/30.0);
    la->addChild(sprite2);
    la->addChild(sprite);
    addChild(la);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto sp=Sprite::create("skillq.png");
    sp->setPosition(801.95,1080-962.45);
    this->addChild(sp);
    sp=Sprite::create("skillw.png");
    sp->setPosition(960.25,1080-953.1);
    this->addChild(sp);
    sp=Sprite::create("skille.png");
    sp->setPosition(1110.85,1080-955.25);
    this->addChild(sp);
    sp=Sprite::create("textq.png");
    sp->setPosition(756.75,1080-912.35);
    this->addChild(sp);
    sp=Sprite::create("textw.png");
    sp->setPosition(912.8,1080-912.35);
    this->addChild(sp);
    sp=Sprite::create("texte.png");
    sp->setPosition(1051.5,1080-912.35);
    this->addChild(sp);
    _qskill=Sprite::create("skillbar.png");
    _qskill->setPosition(810,1080-945.8);
    this->addChild(_qskill);
    _wskill=Sprite::create("skillbar.png");
    _wskill->setPosition(960,1080-945.8);
    this->addChild(_wskill);
    _eskill=Sprite::create("skillbar.png");
    _eskill->setPosition(1110,1080-945.8);
    this->addChild(_eskill);
    sp=Sprite::create("hpbar3.png");
    sp->setAnchorPoint(Vec2(0,1));
    sp->setPosition(0,1080-51.5);
    this->addChild(sp);
    _hpBar1_deal=Sprite::create("hpbar2.png");
    _hpBar1_deal->setAnchorPoint(Vec2(0,1));
    _hpBar1_deal->setPosition(0,1080-51.5);
    this->addChild(_hpBar1_deal);
    _hpBar1_real=Sprite::create("hpbar.png");
    _hpBar1_real->setAnchorPoint(Vec2(0,1));
    _hpBar1_real->setPosition(0,1080-51.5);
    this->addChild(_hpBar1_real);
    sp=Sprite::create("hpbar3.png");
    sp->setAnchorPoint(Vec2(0,1));
    sp->setPosition(1020,1080-51.5);
    this->addChild(sp);
    _hpBar2_deal=Sprite::create("hpbar2.png");
    _hpBar2_deal->setAnchorPoint(Vec2(0,1));
    _hpBar2_deal->setPosition(1020,1080-51.5);
    this->addChild(_hpBar2_deal);
    _hpBar2_real=Sprite::create("hpbar.png");
    _hpBar2_real->setAnchorPoint(Vec2(0,1));
    _hpBar2_real->setPosition(1020,1080-51.5);
    this->addChild(_hpBar2_real);
    sp=Sprite::create("time.png");
    sp->setPosition(960,1080-101.5);
    this->addChild(sp);
    _vs = Label::createWithTTF("0", "fonts/abc.otf", 40);
    _vs->setPosition(960,1080-101.5);
    _vs->setColor(ccc3(255,255,255));
    this->addChild(_vs);
    _qskill->setOpacity(0);
    _wskill->setOpacity(0);
    _eskill->setOpacity(0);
    count=0;
    return true;
}


void GameScene::updateConsole(float dt)
{
    count++;
    int time=count/30;
    _vs->setString(tostr(time));
    if(time==100)
    {
        int winlost=Comander::getInstance()->win();
        auto scene=EndScene::create(Comander::getInstance()->getCon1(),Comander::getInstance()->getCon2(),winlost);
        cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(0.5,scene, cocos2d::Color3B(0,0,0)));
    }
    if(_hpBar1_deal->numberOfRunningActions()==0&&_actionStack1.size()!=0)
    {
        _hpBar1_deal->runAction(_actionStack1.front());
        _actionStack1.front()->release();
        _actionStack1.pop();
    }
    if(_hpBar2_deal->numberOfRunningActions()==0&&_actionStack2.size()!=0)
    {
        _hpBar2_deal->runAction(_actionStack2.front());
        _actionStack2.front()->release();
        _actionStack2.pop();
    }
    _console->update();
}
void GameScene::notify(int id,std::string str)
{
    string* results=StringSplit(str,",");
    //CCLog("%s",results[0].c_str());
    if(results[0]=="kill")
    {
        int winlost=Comander::getInstance()->win();
        auto scene=EndScene::create(Comander::getInstance()->getCon1(),Comander::getInstance()->getCon2(),winlost);
        cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(0.5,scene, cocos2d::Color3B(0,0,0)));
    }
    if(id==1)
    {
        if(results[0]=="dmg")
        {
            float before=_hpBar1_real->getScaleX();
            _hpBar1_real->setScaleX(_console->player1()->getHp()/_console->player1()->getInfo().initialHp);
            float factor=(before-_hpBar1_real->getScaleX());
            auto action=EaseExponentialOut::create(ScaleTo::create(factor*5,_hpBar1_real->getScaleX(),1));
            _actionStack1.push(action);
            action->retain();
        }
        if(results[0]=="genhp")
        {
            _hpBar1_real->setScaleX(_console->player1()->getHp()/_console->player1()->getInfo().initialHp);
        }
        if(results[0]=="qcool")
        {
            
            _qskill->setOpacity(255*((float)_console->player1()->qCool()/_console->player1()->getInfo().qCoolDown));
        }
        if(results[0]=="wcool")
        {
            _wskill->setOpacity(255*((float)_console->player1()->wCool()/_console->player1()->getInfo().wCoolDown));
        }
        
        if(results[0]=="ecool")
        {
            _eskill->setOpacity(255*((float)_console->player1()->eCool()/_console->player1()->getInfo().eCoolDown));
        }
    }
    if(id==2)
    {
        if(results[0]=="dmg")
        {
            float before=_hpBar2_real->getScaleX();
            _hpBar2_real->setScaleX(_console->player2()->getHp()/_console->player2()->getInfo().initialHp);
            float factor=(before-_hpBar2_real->getScaleX());
            auto action=EaseExponentialOut::create(ScaleTo::create(factor*5,_hpBar2_real->getScaleX(),1));
            _actionStack2.push(action);
            action->retain();
        }
        if(results[0]=="genhp")
        {
            _hpBar2_real->setScaleX(_console->player2()->getHp()/_console->player2()->getInfo().initialHp);
        }
    }
}
