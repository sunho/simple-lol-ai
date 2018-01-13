    //
//  NewController.cpp
//  AI
//
//  Created by sunho kim on 2017/11/14.
//

#include "NewController.hpp"
#include <cocos2d.h>
#include <vector>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <stdlib.h>
#define PI 3.14159265
namespace python = boost::python;

template <class T>
inline
boost::python::list std_vector_to_py_list(std::vector<T> vector) {
    typename std::vector<T>::iterator iter;
    boost::python::list list;
    for (iter = vector.begin(); iter != vector.end(); ++iter) {
        list.append(*iter);
    }
    return list;
}
float NewController::getReward()
{
    float r=_reward;
    _reward=0;
    return r;
}
NewController::NewController(std::string name,int training)
{
    _step=0;
    _avoiding=0;
    _asdf=false;
    _totalreward=0;
    _doing=0;
    _agent = python::import("DQNAgent");
    _module=_agent.attr("DQN")(16,23,name,training);
}
NewController::NewController(python::object module)
{
    _avoiding=0;
    _totalreward=0;
    _doing=0;
    _module=module;
}
void NewController::notify(int id,std::string str)
{
    std::string* results=StringSplit(str,",");
    ///CCLog("%s",str.c_str());
    if(results[0]=="dmg")
    {
        _reward-=std::stof(results[1]);
    }
    if(results[0]=="hit")
    {
        _reward+=std::stof(results[1]);
        _tick=8;
    }
    if(results[0]=="kill")
    {
        end();
    }
    if(results[0]=="death")
    {
        end();
    }
    if(results[0]=="anemyq")
    {
       // calculate();
    }
}
void NewController::calculate()
{
    if(_player->getOpp()->getQCircle().dis>0)
    {
        QCircle _qcircle=_player->getOpp()->getQCircle();
        float speed=_qcircle.speed;
        float dis=_qcircle.dis;
        int candidates[16];
        int scope=dis/speed+2;
        float px;
        float py;
        for(int i=0;i<16;i++) candidates[i]=-1;
        for(int j=0;j<16;j++)
        {
            int doing=_player->remainingDoing();
            int d=0;
            for(int i=0;i<scope;i++)
            {
                
                if(doing==0)
                {
                    px=adjustX(_player->getX()+moveX(_player->getRadi(j),_player->getInfo().speed*d),_player->getInfo().size);
                    py=adjustY(_player->getY()+moveY(_player->getRadi(j),_player->getInfo().speed*d),_player->getInfo().size);
                    d++;
                }
                else
                {
                    doing--;
                    px=_player->getX();
                    py=_player->getY();
                }
                
                for(int z=0;z<scope-i;z++)
                {
                    float x=_qcircle.x+moveX(_qcircle.radi,speed*(z+i+1));
                    float y=_qcircle.y+moveY(_qcircle.radi,speed*(z+i+1));
                    float dis3=sqrt( pow( px-x ,2) + pow( py-y ,2) );
                    if(dis3<=_player->getInfo().size/2+_qcircle.size/2)
                    {
                        candidates[j]=-1;
                        break;
                    }
                    else
                    {
                        candidates[j]=i;
                    }
                }
                if(candidates[j]==i)
                {
                    break;
                }
                
            }
        }
        int mincost=scope+5;
        int minindex=0;
        for(int i=0;i<16;i++)
        {
            if(candidates[i]!=-1)
            {
                if(candidates[i]<mincost)
                {
                    mincost=candidates[i];
                    minindex=i;
                }
            }
        }
        if(mincost==scope+5)
        {
            mincost=0;
        }
        if(mincost!=0) mincost++;
        _avoiding=mincost;
        _avoidingDir=minindex;
    }
}
void NewController::save(float a,int timestep)
{
    _module.attr("write_ep_reward_summary")(a,timestep);
}

std::vector<float> NewController::getState()
{
    std::vector<float> s;
    s.push_back(_player->getX());
    s.push_back(_player->getY());
    s.push_back(_player->getOpp()->getX());
    s.push_back(_player->getOpp()->getY());
    s.push_back(_player->getHp());
    s.push_back(_player->getOpp()->getHp());
    s.push_back(_player->qCool());
    s.push_back(_player->wCool());
    s.push_back(_player->eCool());
    s.push_back(_player->getOpp()->qCool());
    s.push_back(_player->getOpp()->wCool());
    s.push_back(_player->getOpp()->eCool());
    s.push_back(_player->canAttack());
    s.push_back(_player->isWing());
    s.push_back(_player->getOpp()->isWing());
    s.push_back(_player->getOpp()->remainingDoing());
    return s;
}
float NewController::end()
{
    std::vector<float> s=getState();
    python::list list=std_vector_to_py_list(s);
    std::vector<float> s2=_recentstate;
    python::list list2=std_vector_to_py_list(s2);
    _totalreward+=_reward;
    _module.attr("store")(python::tuple(list2),_recentaction,_reward,python::tuple(list),1);
    _reward=0;
    return _totalreward;
}
float NewController::total()
{
    return _totalreward;
}
void NewController::middleQ()
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
void NewController::leftQ()
{
    int dir=_player->getDirection(_player->rotOpp()-22.5);
    _player->q(dir);
}
void NewController::rightQ()
{
    int dir=_player->getDirection(_player->rotOpp()+22.5);
 //   CCLOG("rifghtQ");
    _player->q(dir);
}
void NewController::left2Q()
{
    int dir=_player->getDirection(_player->rotOpp())-2;
    if(dir<0)
    {
        dir+=16;
    }
    
//    CCLOG("left2Q");
    _player->q(dir);
}

void NewController::right2Q()
{
    int dir=_player->getDirection(_player->rotOpp())+2;
    if(dir>16)
    {
        dir-=16;
    }
  //  CCLOG("right2Q");
    _player->q(dir);
}
void NewController::avoidE()
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
void NewController::backE()
{
    int dir=_player->getDirection(_player->rotOpp()+8*22.5);
    _player->e(dir,1000);
}
void NewController::frontE()
{
    int dir=_player->getDirection(_player->rotOpp());
    _player->e(dir,1000);
}
void NewController::train()
{
}
void NewController::scoreAppend(float score)
{
}
void NewController::setName(std::string name)
{
   // _agent.attr("set_name")(name);
}
void NewController::saveImg(std::string path)
{
}
void NewController::realend()
{
    //_module.attr("end");
}
int NewController::getAction()
{
    return _recentaction;
}
void NewController::doAction(int action)
{
    if(action==22)
    {
        _player->w();
        //if(!_player->canW()) _reward-=1;
    }
    else if(action==21)
    {
        _player->attack();
        //  if(!_player->canAttack()) _reward-=1;
    }
    else if(action==20)
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
    else if(action==19)
    {
        int dir=_player->getDirection(_player->rotOpp());
        _player->e(dir,1000);
    }
    else if(action==18)
    {
        int dir=_player->getDirection(_player->rotOpp()+8*22.5);
        _player->e(dir,1000);
    }
    else if(action==17)
    {
        float radi1=_player->getOpp()->getMovingRadi();
        float radi2=_player->rotOpp();
        radi1-=radi2;
        radi1=getTrueRadi(radi1);
        int moving=-1;
        if(_player->disOpp()>=600)
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
                int dir=_player->getDirection(_player->rotOpp()+22.5);
                //   CCLOG("rifghtQ");
                _player->q(dir);
            }
            if(moving==1)
            {
                _player->q(_player->getDirection(_player->rotOpp()));
            }
            if(moving==2)
            {
                int dir=_player->getDirection(_player->rotOpp()-22.5);
                _player->q(dir);
            }
        }
        else
        {
            _player->q(_player->getDirection(_player->rotOpp()));
        }
    }
    else if(action>=1)
    {
        if(_avoiding==0)
        {
            calculate();
            if(_player->getOpp()->getQCircle().dis>0)
            {
                
                QCircle qs=_player->getOpp()->getQCircle();
                bool okay=true;
                int scope=qs.dis/qs.speed;
                int d=1;
                for(int i=0;i<scope+1;i++)
                {
                    float x=adjustX(_player->getX()+moveX(action-1,_player->getInfo().speed*d),_player->getInfo().size);
                    float y=adjustY(_player->getY()+moveY(action-1,_player->getInfo().speed*d),_player->getInfo().size);
                    float cx=qs.x+moveX(qs.radi,qs.speed*(i));
                    float cy=qs.y+moveY(qs.radi,qs.speed*(i+1));
                    float dis3=sqrt( pow( cx-x ,2) + pow( cy-y ,2) );
                    if(d<8)
                    {
                        d++;
                    }
                    if(dis3<=_player->getInfo().size/2+_player->getOpp()->getQCircle().size/2)
                    {
                        okay=false;
                        break;
                    }
                }
                if(!okay)
                {
                    
                }
                else
                {
                    _player->move(action-1,_player->getInfo().speed*8);
                }
            }
            else
            {
                _player->move(action-1,_player->getInfo().speed*8);
            }
        }
        else
        {
            _player->move(action-1,_player->getInfo().speed);
        }
    }
}
void NewController::updates(int timestep)
{
    std::vector<float> s=getState();
    python::list list=std_vector_to_py_list(s);
    calculate();
    if(timestep==0)
    {
        _reward=0;
        _tick=0;
        _totalreward=0;
        _recentstate=s;
        _recentaction=0;
    }
    else
    {
        if(_player->isNothing())
        {
            if(_avoiding==0)
            {
                std::vector<float> s2=_recentstate;
                python::list list2=std_vector_to_py_list(s2);
                if(_tick==8)
                {
                    _tick=0;
                    _module.attr("store")(python::tuple(list2),_recentaction,_reward,python::tuple(list),0);
                    int action=python::extract<int>(_module.attr("action")(python::tuple(list)));
                    doAction(action);
                    _recentaction=action;
                    _step++;
                    if(_step>=161)
                    {
                        _module.attr("train")();
                    }
                    _totalreward+=_reward;
                    _reward=0;
                    _recentstate=s;
                }
                _tick++;
            }
            else
            {
                doAction(1+_avoidingDir);
                _avoiding--;
            }
        }
    }
}
    
