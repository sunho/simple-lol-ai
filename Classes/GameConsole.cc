//
//  Console.cpp
//  AI
//
//  Created by sunho kim on 2017/11/09.
//

#include "GameConsole.hpp"
#include "Commander.hpp"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
namespace python = boost::python;

GameConsole::GameConsole(GameController* controller1,GameController* controller2)
:_controller1(controller1),
_controller2(controller2),
_lostid(0),
_timestep(0),
_end(false),
_record("")
{
    PlayerInfo info;
    info.speed=10;
    info.size=120;
    info.eSize=200;
    info.wSize=400;
    info.attackSize=400;
    info.attackCoolDown=28;
    info.qCoolDown=30*3;
    info.wCoolDown=30*15;
    info.eCoolDown=30*10;
    info.initialHp=600 ;
    info.wDmg=40;
    info.qDmg=150;
    info.genHp=0.15;
    info.eDmg=0; 
    info.attackDmg=50;
    info.eDis=400;
    info.wDuration=120;
    info.eStep1=6;
    info.eStep2=6;
    info.eStep3=9;
    info.attackStep1=3;
    info.attackStep2=9;
    info.qSize=80;
    info.qDis=800;
    info.qSpeed=30;
    info.qStep1=8;
    info.qStep2=12;
    _player1=new Player(1,info);
    _player2=new Player(2,info);
    _player1->addListener(this);
    _player2->addListener(this);
    _player1->addListener(_controller1);
    _player2->addListener(_controller2);
    _player1->setOpp(_player2);
    _player2->setOpp(_player1);
    _controller1->setPlayer(_player1);
    _controller2->setPlayer(_player2);
    _player1->setPosition(300, 500);
    _player2->setPosition(1600, 500);
}
void GameConsole::setRecord(std::string str)
{
    _record=str;
    if(_file.is_open())
    {
        _file.close();
    }
    _file.open(_record,std::ios::out);
    _file<<_controller1->oname()<<"/"<<_controller2->oname()<<"&"<<std::endl;
}
bool GameConsole::update()
{
    if(!_end)
    {
        _controller1->updates(_timestep);
        _controller2->updates(_timestep);
        _player1->update();
        _player2->update();
        _timestep++;
        if(_player1->getHp()<=0||_player2->getHp()<=0)
        {
            _end=true;
            if(_record!="")
            {
                _file<<"hp/"<<_player1->getHp()<<"/"<<_player2->getHp()<<"&"<<std::endl;
            }
            //Comander::getInstance()->interpret("gameend");
        }
        return false;
    }
    else
    {
        return true;
    }
}

Player* GameConsole::player1()
{
    return _player1;
}


Player* GameConsole::player2()
{
    return _player2;
}
GameConsole::~GameConsole()
{
    delete _player1;
    delete _player2;
    if(_file.is_open())
    {
        _file.close();
    }
}
void GameConsole::notify(int id,std::string str)
{
    if(_record!="")
    {
        _file<<_timestep<<"/";
        _file<<str<<"/";
        _file<<id<<"&"<<std::endl;
    }
}
void GameConsole::writeRewards(int reward1,int reward2)
{
    _file<<"rewards/"<<reward1<<"/"<<reward2<<"&"<<std::endl;
}
