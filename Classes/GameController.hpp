//
//  GameController.hpp
//  AI
//
//  Created by sunho kim on 2017/11/06.
//

#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include <fstream>
#include "Player.hpp"
#include "PlayerListener.hpp"
class GameController : public PlayerListener
{
public:
    GameController(): _player(nullptr){};
    virtual void notify(int id,std::string str)=0;
    virtual ~GameController(){};
    virtual std::string oname()=0;
    virtual void updates(int timestep)=0;
    void setPlayer(Player* player){_player=player;};
protected:
    Player* _player;
    int _timestep;
};

#endif /* GameController_hpp */
