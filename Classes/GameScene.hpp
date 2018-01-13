//
//  GameScene.hpp
//  AI
//
//  Created by sunho kim on 2017/11/06.
//

#ifndef GameScene_hpp
#define GameScene_hpp

#include <stdio.h>
#include "Player.hpp"
#include "GameConsole.hpp"
#include "PlayerListener.hpp"
#include <queue>
class GameScene : public cocos2d::Scene, public PlayerListener
{
public:
    virtual bool init(GameConsole* console);
    void updateConsole(float dt);
    virtual void notify(int id,std::string str);
    void gameEnd();
    static GameScene* create(GameConsole* console);
private:
    GameConsole* _console;
    std::queue<cocos2d::Action*>  _actionStack2;
    std::queue<cocos2d::Action*>  _actionStack1;
    cocos2d::Sprite* _qskill;
    cocos2d::Sprite* _wskill;
    cocos2d::Sprite* _eskill;
    cocos2d::Sprite* _hpBar1_real;
    cocos2d::Sprite* _hpBar2_real;
    cocos2d::Sprite* _hpBar1_deal;
    cocos2d::Label* _vs;
    int count;
    cocos2d::Sprite* _hpBar2_deal;
};
#endif /* GameScene_hpp */
