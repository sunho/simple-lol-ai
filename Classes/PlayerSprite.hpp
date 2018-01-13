//
//  PlayerSprite.hpp
//  AI
//
//  Created by sunho kim on 2017/11/06.
//

#ifndef PlayerSprite_hpp
#define PlayerSprite_hpp

#include <stdio.h>
#include <string>
#include <cocos2d.h>
#include "Player.hpp"


class PlayerSprite: public cocos2d::Sprite, PlayerListener
{
public:
    virtual bool init(Player* player);
    virtual void notify(int id, std::string str);
    static PlayerSprite* create(Player* player);
private:
    Player* _player;
    cocos2d::Sprite* _qs;
    cocos2d::Animation* _q;
    cocos2d::Animation* _w;
    cocos2d::Animation* _e;
    cocos2d::Animation* _attack;
    cocos2d::Animation* _stand;
};

#endif /* PlayerSprite_hpp */

