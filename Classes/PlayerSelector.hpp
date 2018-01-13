//
//  PlayerSelecter.hpp
//  AI
//
//  Created by sunho kim on 2017/11/05.
//

#ifndef PlayerSelector_hpp
#define PlayerSelector_hpp

#include <stdio.h>
#include <cocos2d.h>
#include <vector>
class PlayerSelectorGroup;
class PlayerSelector: public cocos2d::Sprite
{
public:
    enum Player
    {
        NewAI,
        OldAI,
        Human
    };
    void setMainPosition(cocos2d::Vec2 vec);
    void reorderMain();
    void goSelecting();
    void goSelected(Player player);
    virtual bool init(PlayerSelectorGroup* group);
    static PlayerSelector* create(PlayerSelectorGroup* group);
    Player getSelected();
private:
    std::vector<cocos2d::Sprite*> _sprites;
    static const cocos2d::Vec2 MiddleItem;
    static const cocos2d::Vec2 LeftItem;
    static const cocos2d::Vec2 RightItem;
    static const cocos2d::Vec2 BackSelecting;
    static const float AnimationDuration;
    static const cocos2d::Vec2 BackScaleSelected;
    cocos2d::Vec2 _mainVec;
    Player _player;
    PlayerSelectorGroup* _group;
};

#endif /* PlayerSelecter_hpp */
