//
//  PlayerSprite.cpp
//  AI
//
//  Created by sunho kim on 2017/11/06.
//

#include "PlayerSprite.hpp"
#include "MainScene.hpp"
using namespace cocos2d;
using namespace std;
#define PI 3.14159265
PlayerSprite* PlayerSprite::create(Player* player)
{
    PlayerSprite *pRet=new(std::nothrow) PlayerSprite();
    if(pRet && pRet->init(player))
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
template<char delimiter>
class WordDelimitedBy : public std::string
{};
Animation* makeAnimation(std::string format, int size)
{
    char str[100] = {0};
    cocos2d::Vector<SpriteFrame*> animFrames;
    for (int i=0; i<=size; i++) {
        sprintf(str,format.c_str(),i);
        
        SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
        
        animFrames.pushBack(frame);
    }
    return Animation::createWithSpriteFrames(animFrames,1.0/30.0);
}
bool PlayerSprite::init(Player* player) {
    _player=player;
    Sprite::initWithFile("player.png");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("player_attack.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("player_q.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("player_e.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("player_w.plist");
    cocos2d::Vector<SpriteFrame*> animFrames;
    animFrames.pushBack(SpriteFrame::create("player.png",Rect(0,0,400,400)));
    _stand=Animation::createWithSpriteFrames(animFrames,1.0/30.0);
    _q=makeAnimation("Player_Q instance 1%04d", 12);
    _w=makeAnimation("Player_W instance 1%04d", 120);
    _e=makeAnimation("Player_E instance 1%04d", 9);
    _qs=Sprite::create("qparti.png");
    _qs->setOpacity(0);
    _attack=makeAnimation("Player_Attack instance 1%04d", 9);
    _stand->retain();
    _qs->retain();
    _q->retain();
    _w->retain();
    _e->retain();
    _attack->retain();
    this->runAction(Animate::create(_stand));
    player->addListener(this);
    setPosition(player->getX(),player->getY());
    return true;
}

void PlayerSprite::notify(int id, std::string str)
{
    string* results=StringSplit(str,",");
    ///CCLog("%s",str.c_str());
    if(results[0]=="move")
    {
        if(!_player->isWing()){
            float xline=std::stof(results[1])-getPositionX();
            float yline=std::stof(results[2])-getPositionY();

                float dis=sqrt(pow(xline,2)+pow(yline,2));
                float radi;
                if(xline>0)
                {
                    radi=90-asin(yline/dis)*(180/PI);
                }
                else if(xline<0)
                {
                    radi=asin(yline/dis)*(180/PI)-90;
                }
            if(xline==0)
            {
                if(yline>0)
                {
                    radi=0;
                }
                if(yline<0)
                {
                    radi=180;
                }
            }
            setRotation(radi);
            
        }
        setPosition(std::stof(results[1]),std::stof(results[2]));
    }
    if(results[0]=="anime")
    {
        if(results[1]=="stand")
        {
            runAction(Animate::create(_stand));
        }
        if(results[1]=="w")
        {
            runAction(Animate::create(_w));
        }
        if(results[1]=="attack")
        {
            runAction(Animate::create(_attack));
            float xline=_player->getOpp()->getX()-getPositionX();
            float yline=_player->getOpp()->getY()-getPositionY();
            float radi;
            if(xline>0)
            {
                radi=90-atan(yline/xline)*(180/PI);
            }
            else if(xline<0)
            {
                radi=260-atan(yline/xline)*(180/PI);
            }
            if(xline==0)
            {
                if(yline>0)
                {
                    radi=0;
                }
                if(yline<0)
                {
                    radi=180;
                }
            }
            setRotation(radi);
        }
        if(results[1]=="e")
        {
            runAction(Animate::create(_e));
        }
        if(results[1]=="q")
        {
            runAction(Animate::create(_q));
        }
    }
    if(results[0]=="qmake")
    {
        getParent()->addChild(_qs);
        _qs->setOpacity(255);
        setRotation(360-(_player->getQCircle().radi-90));
    }
    if(results[0]=="qremove")
    {
        getParent()->removeChild(_qs);
        _qs->setOpacity(0);
    }
    if(results[0]=="qmove")
    {
        _qs->setPosition(std::stof(results[1]),std::stof(results[2]));
    }
}
