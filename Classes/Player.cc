//
//  Player.cpp
//  AI
//
//  Created by sunho kim on 2017/11/06.
//

#include "Player.hpp"

#define CPU_ONLY

#define PI 3.14159265


template <typename T> std::string tostr(const T& t) {
    std::ostringstream os;
    os<<t;
    return os.str();
}

bool Player::moveable()
{
    return _moveable;
}
void Player::setPosition(float x,float y)
{
    _x=adjustX(x,_info.size);
    _y=adjustY(y,_info.size);
    notify(std::string("move,")+tostr(x)+std::string(",")+tostr(y));
}
void Player::dmg(float x,std::string by)
{
    _hp-=x;
    notify("dmg,"+tostr(x)+","+by);
    getOpp()->notify("hit,"+tostr(x));
    if(_hp<=0)
    {
        notify("death");
        getOpp()->notify("kill");
    }
    
}
float Player::getX()
{
    return _x;
}
float Player::getY()
{
    return _y;
}
void Player::move(int dir,float dis)
{
    if(_moveable)
    {
        _currentMovingRadi=getRadi(dir);
        _currentMovingDis=dis;
    }
}
int Player::remainingDoing()
{
    if(_doing == Nothing)
    {
        return 0;
    }
    else if(_doing==Qing)
    {
        return _info.qStep2-_doProgress;
    }
    else if(_doing==Attacking)
    {
         return _info.attackStep2-_doProgress;
    }
    else
    {
        return 0;
    }
}
float Player::getMovingRadi()
{
    return _currentMovingRadi;
}
void Player::stop()
{
    _currentMovingDis=0;
}
float Player::disOpp()
{
    return sqrt( pow( _opp->getX()-getX() ,2) + pow( _opp->getY()-getY() ,2) );
}
bool Player::canAttack()
{
    if(_wing==0&&_doing==Nothing&&_attackCoolDown==0&&disOpp()<=_opp->_info.size/2+_info.attackSize/2)
    {
        return true;
    }
}
void Player::setHp(float hp)
{
    _hp=hp;
}
bool Player::isNothing()
{
    return _doing==Nothing;
}
void Player::attack()
{
    if(canAttack())
    {
        stop();
        _moveable=false;
        _attackCoolDown=_info.attackCoolDown;
        _doing=Attacking;
    }
}

void Player::q(int dir)
{
    if(_qCoolDown==0&&_doing==Nothing)
    {
        stop();
        _doing=Qing;
        _moveable=false;
        _destRadi=getRadi(dir);
        _qCoolDown=_info.qCoolDown;
    }
}

void Player::w()
{
    if(_wCoolDown==0&&_doing==Nothing)
    {
        notify("anime,w");
        _wing=_info.wDuration;
        _wCoolDown=_info.wCoolDown;
    }
}
bool Player::canQ()
{
    return _qCoolDown==0&&_doing==Nothing;
}
bool Player::canW()
{
    return _wCoolDown==0&&_doing==Nothing;
}
bool Player::canE()
{
    return _eCoolDown==0&&_doing==Nothing&&_wing==0;
}
void Player::e(int dir,int dis)
{
    if(_eCoolDown==0&&_doing==Nothing&&_wing==0)
    {
        if(getRealDistance(dis)>_info.eDis)
        {
            dis=getDistance(_info.eDis);
        }
        stop();
        _destRadi=getRadi(dir);
        _destDis=getRealDistance(dis);
        _doing=Eing;
        _moveable=false;
        _eCoolDown=30*10;
    }
}

QCircle Player::getQCircle()
{
    return _qcircle;
}
bool Player::isWing()
{
    return _wing>0;
}
void Player::update()
{
    if(_circling)
    {
        _qcircle.x=_qcircle.x+moveX(getTrueRadi(_qcircle.radi),_qcircle.speed);
        _qcircle.y=_qcircle.y+moveY(getTrueRadi(_qcircle.radi),_qcircle.speed);
        notify("qmove,"+tostr(_qcircle.x)+","+tostr(_qcircle.y));
        _qcircle.dis-=_qcircle.speed;
        float dis=sqrt( pow( _opp->getX()-_qcircle.x ,2) + pow( _opp->getY()-_qcircle.y ,2) );
        if(dis<=_opp->_info.size/2+_qcircle.size/2)
        {
            _opp->dmg(_info.qDmg,"q");
            _qcircle.dis=0;
            _circling=false;
            notify("qremove");
        }
        if(_qcircle.dis<=0)
        {
            _circling=false;
            notify("qremove");
            notify("miss,q");
        }
    }
    if(_currentMovingDis>0)
    {
        _currentMovingDis-=_speed;
        setPosition(_x+moveX(getTrueRadi(_currentMovingRadi),_speed),_y+moveY(getTrueRadi(_currentMovingRadi),_speed));
    }
    else
    {
        _currentMovingDis=0;
        _currentMovingRadi=rotOpp();
    }
    if(_doing==Qing)
    {
        if(_doProgress==0)
        {
            notify("anime,q");
        }
        if(_doProgress==_info.qStep1)
        {
            _circling=true;
            _qcircle.x=_x+moveX(_destRadi,80);
            _qcircle.y=_y+moveY(_destRadi,80);
            _qcircle.radi=_destRadi;
            _qcircle.speed=_info.qSpeed;
            _qcircle.dis=_info.qDis;
            _qcircle.size=_info.qSize;
            getOpp()->notify("anemyq");
            notify("qmake");
        }
        if(_doProgress==_info.qStep2)
        {
            notify("anime,stand");
            _doProgress=0;
            _moveable=true;
            _doing=Nothing;
        }
    }
    if(_doing==Attacking)
    {
        if(_doProgress==0)
        {
            notify("anime,attack");
        }
        if(_doProgress==_info.attackStep1)
        {
            _opp->dmg(_info.attackDmg,"attack");
        }
        if(_doProgress==_info.attackStep2)
        {
            notify("anime,stand");
            _doing=Nothing;
            _doProgress=0;
            _moveable=true;
        }
    }
    if(_doing==Eing)
    {
        if(_doProgress==0)
        {
            notify("anime,e");
        }
        if(_doProgress==_info.eStep1)
        {
            setPosition(_x+moveX(getTrueRadi(_destRadi),_destDis),_y+moveY(getTrueRadi(_destRadi),_destDis));
        }
        if(_doProgress==_info.eStep2)
        {
            if(disOpp()<=_opp->_info.size/2+_info.eSize/2)
            {
                _opp->dmg(_info.eDmg,"e");
            }
        }
        if(_doProgress==_info.eStep3)
        {
            notify("anime,stand");
            _doProgress=0;
            _moveable=true;
            _doing=Nothing;
        }
    }
    if(_wing>0)
    {
        if(_wTick==10)
        {
            if(disOpp()<=_opp->_info.size/2+_info.wSize/2)
            {
                _opp->dmg(_info.wDmg,"w");
            }
            else
            {
                notify("miss,w");
            }
            _wTick=0;
        }
        _wTick++;
        _wing--;
        if(_wing==0)
        {
            _wTick=0;
            notify("anime,stand");
        }
    }
    if(_qCoolDown>0)
    {
        _qCoolDown--;
        notify("qcool,"+tostr(_qCoolDown));
    }
    if(_wCoolDown>0)
    {
        _wCoolDown--;
        notify("wcool,"+tostr(_qCoolDown));
    }
    if(_eCoolDown>0)
    {
        _eCoolDown--;
        notify("ecool,"+tostr(_qCoolDown));
    }
    if(_attackCoolDown>0)
    {
        _attackCoolDown--;
        notify("attackccol,"+tostr(_attackCoolDown));
    }
    if(_doing!=Nothing)
    {
        _doProgress++;
    }
}
bool Player::isCircling()
{
    return _circling;
}
void Player::notify(std::string str)
{
    for(auto iter=_listeners.begin();iter!=_listeners.end();++iter)
    {
        (*iter)->notify(_id,str);
    }
}

Player::Player(int id, PlayerInfo info)
    :_id(id),
    _info(info),
    _hp(_info.initialHp),
    _speed(_info.speed),
    _moveable(true),
    _wing(0),
    _currentMovingRadi(0),
    _currentMovingDis(0),
    _doing(Nothing),
    _destDis(0),
    _destRadi(0),
    _x(0),
    _y(0),
    _doProgress(0),
    _attackCoolDown(0),
    _qCoolDown(0),
    _wCoolDown(0),
    _eCoolDown(0),
    _wTick(0),
_qcircle({0,0,0,0,0,0}),
    _circling(false)
{
}

void Player::addListener(PlayerListener* listener)
{
    _listeners.push_back(listener);
}

void Player::setOpp(Player* player)
{
    _opp = player;
}

Player* Player::getOpp()
{
    return _opp;
}

PlayerInfo Player::getInfo()
{
    return _info;
}

int Player::getDirection(float radi)
{
    int a=floor(getTrueRadi(radi)/25.5+0.5);
    if(a==16) a=0;
    return a;
}

float Player::getRadi(int dir)
{
    return dir*25.5;
}

int Player::getDistance(float distance)
{
    return floor(distance/_info.speed+0.5);
}

int Player::getRealDistance(int dis)
{
    return dis*_info.speed;
}

float Player::getHp()
{
    return _hp;
}

int Player::qCool()
{
    return _qCoolDown;
}

int Player::wCool()
{
    return _wCoolDown;
}

int Player::eCool()
{
    return _eCoolDown;
}
int Player::id()
{
    return _id;
}
float Player::rotOpp()
{
    float xline=getOpp()->getX()-getX();
    float yline=getOpp()->getY()-getY();
    float radi;
    if(xline>0)
    {
        radi=atan(yline/xline)*(180/PI);
    }
    else if(xline<0)
    {
        radi=atan(yline/xline)*(180/PI)+180;
    }
    if(xline==0)
    {
        if(yline>0)
        {
            radi=90;
        }
        if(yline<0)
        {
            radi=-90;
        }
    }
    return getTrueRadi(radi);
}
