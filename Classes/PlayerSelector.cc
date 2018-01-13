    //
//  PlayerSelecter.cpp
//  AI
//
//  Created by sunho kim on 2017/11/05.
//

#include "PlayerSelector.hpp"
#include "PlayerSelectorGroup.hpp"
using namespace cocos2d;

const Vec2 PlayerSelector::BackScaleSelected=Vec2(0.32,0.64);
const Vec2 PlayerSelector::MiddleItem=Vec2(960,1080-540);
const Vec2 PlayerSelector::RightItem=Vec2(960-500,1080-540);
const Vec2 PlayerSelector::LeftItem=Vec2(960+500,1080-540);
const Vec2 PlayerSelector::BackSelecting=Vec2(960,1080-540);
const float PlayerSelector::AnimationDuration=0.5;
PlayerSelector* PlayerSelector::create(PlayerSelectorGroup* group)
{
    PlayerSelector *pRet=new(std::nothrow) PlayerSelector();
    if(pRet && pRet->init(group))
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

bool PlayerSelector::init(PlayerSelectorGroup* group)
{
    Sprite::init();
    _player=OldAI;
    if(group){
        _group=group;
    }
    else
    {
        return false;
    }
    _sprites.push_back(Sprite::create("none.png"));
    _sprites.push_back(Sprite::create("back.png"));
    _sprites.push_back(Sprite::create("newai.png"));
    _sprites.push_back(Sprite::create("oldai.png"));
    _sprites.push_back(Sprite::create("human.png"));
    _sprites[1]->setScale(BackScaleSelected.x,BackScaleSelected.y);
    for(auto iter=_sprites.begin();iter !=_sprites.end();++iter){
        (*iter)->setPosition(_mainVec);
        (*iter)->setAnchorPoint(Vec2(0.5,0.5));
        this->addChild(*iter);
    }
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        return true;
    };
    listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        cocos2d::Vec2 p = touch->getLocation();
        if(_group->getClickable())
        {
            if(_group->getSelectingPlayer()==nullptr)
            {
                cocos2d::Rect rect = _sprites[0]->getBoundingBox();
                if(rect.containsPoint(p))
                {
                    this->goSelecting();
                }
            }
            else if(_group->getSelectingPlayer()==this)
            {
                cocos2d::Rect rect1 = _sprites[2]->getBoundingBox();
                cocos2d::Rect rect2 = _sprites[3]->getBoundingBox();
                cocos2d::Rect rect3 = _sprites[4]->getBoundingBox();
                if(rect1.containsPoint(p))
                {
                    this->goSelected(NewAI);
                }
                if(rect2.containsPoint(p))
                {
                   this->goSelected(OldAI);
                }
                if(rect3.containsPoint(p))
                {
                    this->goSelected(Human);
                }
            }
        }
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    reorderMain();
    return true;
}

void PlayerSelector::setMainPosition(cocos2d::Vec2 vec) { 
    if(_group->getSelectingPlayer()==nullptr)
    {
        _mainVec=vec;
        for(auto iter=_sprites.begin();iter !=_sprites.end();++iter){
            (*iter)->setPosition(_mainVec);
        }
    }
}

void PlayerSelector::reorderMain() {
    bool leftPlayed=false;
    for(int i=0;i<3;i++)
    {
        if(i==_player)
        {
        }
        else if(!leftPlayed)
        {
            reorderChild(_sprites[2+i],0);
            leftPlayed=true;
        }
        else
        {
            reorderChild(_sprites[2+i],0);
        }
    }
    reorderChild(_sprites[2+_player],0);
}

void PlayerSelector::goSelecting() {
    _group->setClickable(false);
    auto func=CallFunc::create([=](){this->_group->setClickable(true);});
    _group->reorderChild(this, 0);
    _group->setSelectingPlayer(this);
    auto mainMove=MoveTo::create(AnimationDuration, MiddleItem);
    auto leftMove=MoveTo::create(AnimationDuration, LeftItem);
    auto rightMove=MoveTo::create(AnimationDuration,RightItem);
    auto small1=ScaleTo::create(AnimationDuration,1.2);
    auto small2=ScaleTo::create(AnimationDuration,0.8);
    auto large=ScaleTo::create(AnimationDuration,1);
    auto backMove=MoveTo::create(AnimationDuration,BackSelecting);
    auto main=EaseExponentialInOut::create(Spawn::create(mainMove,small1,NULL));
    auto left=EaseExponentialInOut::create(Spawn::create(leftMove,small2,NULL));
    auto right=EaseExponentialInOut::create(Spawn::create(rightMove,small2->clone(),NULL));
    auto back=Sequence::create(EaseExponentialInOut::create(Spawn::create(backMove,large,NULL)),func,NULL);
    bool leftPlayed=false;
    for(int i=0;i<3;i++)
    {
        if(i==_player)
        {
            _sprites[2+i]->runAction(main);
        }
        else if(!leftPlayed)
        {
            _sprites[2+i]->runAction(left);
            leftPlayed=true;
        }
        else
        {
            _sprites[2+i]->runAction(right);
        }
    }
    _sprites[1]->runAction(back);
}

void PlayerSelector::goSelected(PlayerSelector::Player player) {
    _group->setClickable(false);
    auto func=CallFunc::create([=](){this->_group->setClickable(true);});
    _group->setSelectingPlayer(nullptr);
    auto move=MoveTo::create(AnimationDuration,_mainVec);
    auto large=ScaleTo::create(AnimationDuration,1);
    auto small=ScaleTo::create(AnimationDuration,BackScaleSelected.x,BackScaleSelected.y);
    for(int i=0;i<3;i++)
    {
        _sprites[2+i]->runAction(EaseExponentialInOut::create(Spawn::create(move->clone(),large->clone(),NULL)));
    }
    _sprites[1]->runAction(Sequence::create(EaseExponentialInOut::create(Spawn::create(move->clone(),small,NULL)),func,NULL));
    _player=player;
    reorderMain();
}

PlayerSelector::Player PlayerSelector::getSelected()
{
    return _player;
}



