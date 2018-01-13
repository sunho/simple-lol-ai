//
//  OldController.cpp
//  AI
//
//  Created by sunho kim on 2017/11/10.
//

#include "OldController.hpp"
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
float OldController::end()
{
    std::vector<float> s=getState();
    python::list list=std_vector_to_py_list(s);
    std::vector<float> s2=_recentstate;
    python::list list2=std_vector_to_py_list(s2);
    _totalreward+=_reward;
  //  _module.attr("store")(python::tuple(list2),_recentaction,_reward,python::tuple(list),1);
    _reward=0;
    return _totalreward;
}

void OldController::calculate()
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
void OldController::notify(int id,std::string str)
{
    std::string* results=StringSplit(str,",");
    if(results[0]=="dmg")
    {
        _reward-=std::stof(results[1]);
    }
    if(results[0]=="hit")
    {
        _reward+=std::stof(results[1]);
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
        calculate();
    }
}

void OldController::setType(int type)
{
    _type=type;
}
std::vector<float> OldController::getState()
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
void OldController::updates(int timestep)
{
    int action=getAction();
    std::vector<float> s=getState();
    python::list list=std_vector_to_py_list(s);
    calculate();
    if(timestep==0)
    {
        ds=30;
        _tick=0;
        _reward=0;
        _totalreward=0;
        _recentstate=s;
        _recentaction=0;
    }
    if(_avoiding>0)
    {
        _avoiding--;
        action=1+_avoidingDir;
        doAction(action);
    }
    else if(_player->isNothing())
    {
        std::vector<float> s2=_recentstate;
        python::list list2=std_vector_to_py_list(s2);
       // _module.attr("store")(python::tuple(list2),_recentaction,_reward,python::tuple(list),0);
        doAction(action);
        _recentaction=action;
        _reward=0;
        _recentstate=s;
    }
    
    
}
float areaX(float x)
{
    float areax=-1;
    if(0<x&&x<200)
    {
        areax=x/200;
    }
    else if(200<=x&&x<500)
    {
        areax=1+(x-200)/300;
    }
    else if(500<=x&&x<960)
    {
        areax=2+(x-500)/460;
    }
    else if(960<=x&&x<1420)
    {
        areax=3+(x-960)/460;
    }
    else if(1420<=x&&x<1720)
    {
        areax=4+(x-1420)/300;
    }
    else
    {
        areax=5+(x-1720)/200;
    }
    return areax;
}
float areaY(float y)
{
    float areay=-1;
    if(0<y&&y<200)
    {
        areay=y/200;
    }
    else if(200<=y&&y<400)
    {
        areay=1+(y-200)/200;
    }
    else if(400<=y&&y<540)
    {
        areay=2+(y-400)/140;
    }
    else if(540<=y&&y<680)
    {
        areay=3+(y-540)/140;
    }
    else if(680<=y&&y<880)
    {
        areay=4+(y-680)/200;
    }
    else
    {
        areay=5+(y-880)/200;
    }
    return areay;
}
int properDir(int dir)
{
    if(dir>16) return dir-16;
    if(dir<0) return dir+16;
    return dir;
}
int OldController::backward()
{
    float mostDis=0;
    int mostRadi=0;
    for(int i=0;i<16;i++)
    {
        float x=adjustX(_player->getX()+moveX(_player->getRadi(i),_player->getInfo().speed),_player->getInfo().size);
        float y=adjustY(_player->getY()+moveY(_player->getRadi(i),_player->getInfo().speed),_player->getInfo().size);
        float dis=sqrt(pow(x-_player->getOpp()->getX(),2)+pow(y-_player->getOpp()->getY(),2));
        if(mostDis<dis)
        {
            mostDis=dis;
            mostRadi=i;
        }
    }
    return 1+mostRadi;
}
void OldController::doAction(int action)
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
            if(_player->getOpp()->getQCircle().dis>0)
            {
                float x=adjustX(_player->getX()+moveX(action-1,_player->getInfo().speed*2),_player->getInfo().size);
                float y=adjustY(_player->getY()+moveY(action-1,_player->getInfo().speed*2),_player->getInfo().size);
                QCircle qs=_player->getOpp()->getQCircle();
                bool okay=true;
                int scope=qs.dis/qs.speed;
                for(int i=0;i<scope+1;i++)
                {
                    float cx=qs.x+moveX(qs.radi,qs.speed*(i));
                    float cy=qs.y+moveY(qs.radi,qs.speed*(i));
                    float dis3=sqrt( pow( cx-x ,2) + pow( cy-y ,2) );
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
                    _player->move(action-1,_player->getInfo().speed);
                }
            }
            else
            {
                _player->move(action-1,_player->getInfo().speed);
            }
        }
        else
        {
            _player->move(action-1,_player->getInfo().speed);
        }
    }
}
float OldController::killAngle(Player* player)
{
    if(player->disOpp()<300)
    {
        return player->canQ()*player->getInfo().qDmg+player->canW()*player->getInfo().wDmg*2+player->getInfo().attackDmg+player->isWing()*player->getInfo().wDmg*2+_player->getInfo().attackDmg;
    }
    else
    {
        return player->canQ()*player->getInfo().qDmg+player->canW()*player->getInfo().wDmg*3+_player->getInfo().attackDmg;
    }
}
OldController::OldController(boost::python::object module)
{
    _module=module;
    _avoiding=0;
}
int OldController::getAction()
{
    int st1=700;
    int st2=500;
    int st3=250;
    if(_player->disOpp()>st1)
    {
        if(ds>0)
        {
            ds--;
            if(rand()%100>50)
            {
                return 1+4;
            }
            else
            {
                
                return 1+11;
            }
            
        }
        else
        {
            return 1+_player->getDirection(_player->rotOpp());
        }
        
    }
    else if(_player->disOpp()<st3)
    {
        if(_player->canE()&&_player->getHp()<killAngle(_player->getOpp()))
        {
            return 18;
        }
        if(_player->canW()&&(_player->getOpp()->eCool()!=0||killAngle(_player)>_player->getOpp()->getHp()))
        {
            return 22;
        }
        if(_player->canAttack())
        {
            return 21;
        }
        if(_player->canQ())
        {
            return 17;
        }
        if(_player->disOpp()>60)
        {
            return 1+_player->getDirection(_player->rotOpp());
        }
        if(_player->disOpp()<60)
        {
            return 1+_player->getDirection(_player->rotOpp()+180);
        }
        
    }
    else if(_player->disOpp()<st1)
    {
        if(_player->disOpp()<st2)
        {
            if(_player->canE())
            {
                if(killAngle(_player)>_player->getOpp()->getHp())
                {
                    return 19;
                }
                if(killAngle(_player->getOpp())>_player->getHp())
                {
                    return backward();
                }
            }
        }
        if(_player->getOpp()->canQ()&&(!_player->canQ()))
        {
            return backward();
        }
        else if((!_player->getOpp()->canQ())&&(!_player->canQ()))
        {
            if(_player->qCool()<_player->qCool()||_player->getHp()>_player->getOpp()->getHp())
            {
                return 1+_player->getDirection(_player->rotOpp());
            }
            if(_player->qCool()>=_player->qCool())
            {
                return backward();
            }
            if(_player->disOpp()>700)
            {
                return 1+_player->getDirection(_player->rotOpp());
            }
            else
            {
                return backward();
            }
        }
        else
        {
            if(_player->disOpp()<=500)
            {
                return 17;
            }
            else
            {
                return 1+_player->getDirection(_player->rotOpp());
            }
        }
    }
    
    return 0;
}
