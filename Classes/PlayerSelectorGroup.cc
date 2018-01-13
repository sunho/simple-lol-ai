//
//  PlayerSelecter.cpp
//  AI
//
//  Created by sunho kim on 2017/11/05.
//

#include "PlayerSelectorGroup.hpp"
using namespace cocos2d;

bool PlayerSelectorGroup::init()
{
    Sprite::init();
    _clickable=true;
    _selectingPlayer=nullptr;
    _player1=PlayerSelector::create(this);
    _player2=PlayerSelector::create(this);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _player1->setMainPosition(Vec2(523,visibleSize.height-550));
    _player2->setMainPosition(Vec2(1395,visibleSize.height-550));
    this->addChild(_player1);
    this->addChild(_player2);
    return true;
}

bool PlayerSelectorGroup::getClickable()
{
    return _clickable;
}

void PlayerSelectorGroup::setClickable(bool c)
{
    _clickable=c;
}

PlayerSelector* PlayerSelectorGroup::getSelectingPlayer()
{
    return _selectingPlayer;
};
void PlayerSelectorGroup::setSelectingPlayer(PlayerSelector* player)
{
    _selectingPlayer=player;
};

PlayerSelector* PlayerSelectorGroup::player1()
{
    return _player1;
}

PlayerSelector* PlayerSelectorGroup::player2()
{
    return _player2;
}
