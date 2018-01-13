//
//  Console.hpp
//  AI
//
//  Created by sunho kim on 2017/11/09.
//

#ifndef Console_hpp
#define Console_hpp

#include <stdio.h>
#include "PlayerListener.hpp"
#include "Player.hpp"
#include "GameController.hpp"
class GameConsole : public PlayerListener
{
public:
    GameConsole(GameController* controller1,GameController* controller2);
    ~GameConsole();
    bool update();
    void writeRewards(int reward1,int reward2);
    int lostId();
    virtual void notify(int id,std::string str);
    void setRecord(std::string str);
    Player* player1();
    Player* player2();
private:
    Player* _player1;
    Player* _player2;
    int _lostid;
    std::ofstream _file;
    std::string _record;
    bool _end;
    GameController* _controller1;
    GameController* _controller2;
    int _timestep;
};
#endif /* Console_hpp */
