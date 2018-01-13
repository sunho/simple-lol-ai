//
//  NewDummyController.cpp
//  AI
//
//  Created by sunho kim on 2017/12/05.
//

#include "NewDummyController.hpp"


void NewDummyController::doAction(int action)
{
    if(action==22)
    {
        _player->w();
    }
    else if(action==21)
    {
        _player->attack();
    }
    else if(action==20)
    {
        avoidE();
    }
    else if(action==19)
    {
        frontE();
    }
    else if(action==18)
    {
        backE();
    }
    else if(action==17)
    {
        middleQ();
    }
    else if(action>=1)
    {
        _player->move(action-1,_player->getInfo().speed);
    }
}
void NewDummyController::middleQ()
{
    float radi1=_player->getOpp()->getMovingRadi();
    float radi2=_player->rotOpp();
    radi1-=radi2;
    radi1=getTrueRadi(radi1);
    int moving=-1;
    if(_player->disOpp()>=120)
    {
        if(45<=radi1&&radi1<=135)
        {
            moving=0;
        }
        else if(135<radi1&&radi1<=225)
        {
            moving=1;
        }
        else if(225<radi1&&radi1<=315)
        {
            moving=2;
        }
        else
        {
            moving=3;
        }
        if(moving==3)
        {
            _player->q(_player->getDirection(_player->rotOpp()));
        }
        if(moving==0)
        {
            rightQ();
        }
        if(moving==1)
        {
            _player->q(_player->getDirection(_player->rotOpp()));
        }
        if(moving==2)
        {
            leftQ();
        }
    }
    else
    {
        _player->q(_player->getDirection(_player->rotOpp()));
    }
    // CCLOG("q");
}
void NewDummyController::leftQ()
{
    int dir=_player->getDirection(_player->rotOpp()-22.5);
    _player->q(dir);
}
void NewDummyController::rightQ()
{
    int dir=_player->getDirection(_player->rotOpp()+22.5);
    //   CCLOG("rifghtQ");
    _player->q(dir);
}
void NewDummyController::avoidE()
{
    Player* opp=_player->getOpp();
    float dis=_player->getInfo().eDis;
    int leftdir=_player->getDirection(_player->rotOpp()+4*22.5);
    int rightdir=_player->getDirection(_player->rotOpp()-4*22.5);
    int size=_player->getInfo().size/2;
    float x=_player->getX()+moveX(_player->getRadi(leftdir),dis);
    float y=_player->getY()+moveY(_player->getRadi(leftdir),dis);
    float x1=_player->getX()+moveX(_player->getRadi(rightdir),dis);
    float y1=_player->getY()+moveY(_player->getRadi(rightdir),dis);
    if(x<1920-size&&x>size&&y<1080-size&&y>size)
    {
        _player->e(leftdir,1000);
    }
    else if(x1<1920-size&&x1>size&&y1<1080-size&&y1>size)
    {
        _player->e(rightdir,1000);
    }
    else
    {
        _player->e(leftdir,1000);
    }
}
void NewDummyController::backE()
{
    int dir=_player->getDirection(_player->rotOpp()+8*22.5);
    _player->e(dir,1000);
}
void NewDummyController::frontE()
{
    int dir=_player->getDirection(_player->rotOpp());
    _player->e(dir,1000);
}
