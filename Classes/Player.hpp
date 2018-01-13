//
//  Player.hpp
//  AI
//
//  Created by sunho kim on 2017/11/06.
//

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>
#include <cocos2d.h>
#include <memory>
#include <list>
#include <boost/python.hpp>

#include "PlayerListener.hpp"
#define PI 3.14159265
typedef struct
{
    float speed;
    float size;
    int qCoolDown;
    int wCoolDown;
    int eCoolDown;
    int attackCoolDown;
    float initialHp;
    float wSize;
    float attackSize;
    float eSize;
    float eDis;
    float eDmg;
    float qDmg;
    float qSize;
    float attackDmg;
    float wDmg;
    float wDuration;
    float qSpeed;
    float qDis;
    int attackStep1;
    int attackStep2;
    int eStep1;
    int eStep2;
    int eStep3;
    float genHp;
    int qStep1;
    int qStep2;
}PlayerInfo;
typedef struct
{
    float x;
    float y;
    float radi;
    float dis;
    float size;
    float speed;
}QCircle;

class Player
{
public:
    enum Doing
    {
        Nothing,
        Attacking,
        Qing,
        Eing
    };
    Player(){}
    Player(int id, PlayerInfo info);
    ~Player(){};
    void addListener(PlayerListener* listner);
    int id();
    QCircle getQCircle();
    void setHp(float hp);
    float getHp();
    float getX();
    float getY();
    void setPosition(float x,float y);
    void setX(float x);
    void setY(float y);
    Player* getOpp();
    bool isNothing();
    bool moveable();
    void dmg(float x,std::string by);
    void setOpp(Player* player);
    float getRadi(int dir);
    int getRealDistance(int dis);
    int getDirection(float radi);
    int getDistance(float dis);
    void move(int dir,float dis);
    float getMovingRadi();
    void stop();
    float disOpp();
    int qCool();
    int wCool();
    int eCool();
    bool canQ();
    bool canW();
    bool canE();
    bool isWing();
    int attckCool();
    int remainingDoing();
    bool isCircling();
    PlayerInfo getInfo();
    bool canAttack();
    void attack();
    void q(int dir);
    float rotOpp();
    void w();
    void e(int radi, int dis);
    void update();
private:
    void notify(std::string str);
    std::vector<PlayerListener*> _listeners;
    Player* _opp;
    PlayerInfo _info;
    QCircle _qcircle;
    bool _circling;
    float _currentMovingRadi;
    int _id;
    int _wTick;
    int _attackCoolDown;
    int _qCoolDown;
    int _wCoolDown;
    float _speed;
    int _eCoolDown;
    float _currentMovingDis;
    Doing _doing;
    float _destRadi;
    float _destDis;
    float _x;
    float _y;
    int _doProgress;
    int _wing;
    bool _moveable;
    float _hp;
};
static float getTrueRadi(float radi)
{
    if(radi<0)
    {
        radi=radi+360;
    }
    if(radi>=360)
    {
        radi-=(radi/360)*360;
    }
    return radi;
}
static float adjustX(float _x, float _size)
{
    if(_x<_size/2)
    {
        _x=_size/2;
    }
    if(_x>1920-_size/2)
    {
        _x=1920-_size/2;
    }
    return _x;
}
static float adjustY(float _y, float _size)
{
    if(_y<_size/2)
    {
        _y=_size/2;
    }
    if(_y>1080-_size/2)
    {
        _y=1080-_size/2;
    }
    return _y;
}
static float moveX(float radi,float dis)
{
    if(radi==0)
    {
        return dis;
    }
    else if(radi==90)
    {
        return 0;
    }
    else if(radi==180)
    {
        return -dis;
    }
    else if(radi==270)
    {
        return 0;
    }
    return cos(radi*PI/180.00)*dis;
}
static float moveY(float radi,float dis)
{
    if(radi==0)
    {
        return 0;
    }
    else if(radi==90)
    {
        return dis;
    }
    else if(radi==180)
    {
        return 0;
    }
    else if(radi==270)
    {
        return -dis;
    }
    return sin(radi*PI/180.00)*dis;
}

#endif /* Player_hpp */
