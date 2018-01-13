//
//  PlayerSelecter.hpp
//  AI
//
//  Created by sunho kim on 2017/11/05.
//

#ifndef PlayerSelectorGroup_hpp
#define PlayerSelectorGroup_hpp

#include <stdio.h>
#include <cocos2d.h>
#include "PlayerSelector.hpp"

class PlayerSelectorGroup: public cocos2d::Sprite
{
public:
    virtual bool init();
    void setClickable(bool c);
    bool getClickable();
    PlayerSelector* getSelectingPlayer();
    void setSelectingPlayer(PlayerSelector* player);
    PlayerSelector* player1();
    PlayerSelector* player2();
    CREATE_FUNC(PlayerSelectorGroup);
private:
    PlayerSelector* _player1;
    PlayerSelector* _player2;
    PlayerSelector* _selectingPlayer;
    bool _clickable;
};

#endif /* PlayerSelecter_hpp */
