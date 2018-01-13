//
//  Commander.hpp
//  AI
//
//  Created by sunho kim on 2017/11/29.
//

#ifndef Commander_hpp
#define Commander_hpp
#include "GameConsole.hpp"
#include "NewController.hpp"
#include "ReplayController.hpp"
#include "GameController.hpp"
#include "MouseController.hpp"
#include "GameScene.hpp"
#include "MainScene.hpp"
#include "NewDummyController.hpp"
#include "OldController.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
namespace python = boost::python;

template <typename T> std::string tostr(const T& t) {
    std::ostringstream os;
    os<<t;
    return os.str();
}
using boost::asio::ip::tcp;
class Comander
{
public:
    static Comander* getInstance()
    {
        if(_instance==nullptr)
            _instance=new Comander();
        return _instance;
    }
    void destroy()
    {
        delete _instance;
    }
    std::string getCon1()
    {
        return _con1;
    }
    
    std::string getCon2()
    {
        return _con2;
    }
    std::string interesting()
    {
        int mostindex=0;
        int most=-1;
        for(int i=0;i<_wins.size();i++)
        {
            if(most<_wins[i])
            {
                mostindex=i;
                most=_wins[i];
            }
        }
        if(mostindex==0)
        {
            return std::string("interesting fact! today traditional AI is dominating the simple AI world with ")+tostr(_wins[0])+"wins. human:"+tostr(_wins[2])+"deep:"+tostr(_wins[1]);
        }
        else if(mostindex==1)
        {
            return std::string("interesting fact! today deep learning AI is dominating the simple AI world with ")+tostr(_wins[1])+"wins. trad:"+tostr(_wins[0])+"human:"+tostr(_wins[2]);
        }
        else
        {
            return std::string("interesting fact! today human is dominating the simple AI world with ")+tostr(_wins[2])+"wins. trad:"+tostr(_wins[0])+"deep:"+tostr(_wins[1]);
        }
    }
    int win()
    {
        int winlost=0;
        if(_console->player1()->getHp()>_console->player2()->getHp())
        {
            winlost=1;
        }
        else if(_console->player1()->getHp()<_console->player2()->getHp())
        {
            winlost=-1;
        }
        if(winlost==1)
        {
            if(_con1=="trad")
            {
                _wins[0]++;
            }
            else if(_con1=="deep")
            {
                _wins[1]++;
            }
            else
            {
                _wins[2]++;
            }
        }
        else if(winlost==-1)
        {
            if(_con2=="trad")
            {
                _wins[0]++;
            }
            else if(_con2=="deep")
            {
                _wins[1]++;
            }
            else
            {
                _wins[2]++;
            }
        }
        return winlost;
    }
    void interpret(std::string str)
    {
        std::string* results=StringSplit(str,",");
        if(results[0]=="viewgame")
        {
            if(_newControler==nullptr) _newControler=new NewController("ad5",1);
            _controller1=getConByString(results[1]);
            _controller2=getConByString(results[2]);
            _console=new GameConsole(_controller1,_controller2);
            _console->setRecord("/Users/sunhokim/Schoolworks/AI/Resources/replay/replay"+results[1]+"vs"+results[2]+tostr(time(NULL))+".txt");
            auto scene=GameScene::create(_console);
            if(results[1]=="human")
            {
                scene->addChild(dynamic_cast<cocos2d::Sprite*>(_controller1));
                
            }
            else if(results[2]=="human")
            {
                scene->addChild(dynamic_cast<cocos2d::Sprite*>(_controller2));
            }
            cocos2d::Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(0.5,scene, cocos2d::Color3B(0,0,0)));
            _con1=results[1];
            _con2=results[2];
        }
        if(results[0]=="train")
        {
            int dd=0;
            int number=std::stoi(results[1]);
            int training=std::stoi(results[2]);
            _controller1=new NewController(results[3],training);
            NewController* p1=dynamic_cast<NewController*>(_controller1);
            _controller2=new OldController(p1->_module);
            int p1w=0;
            int p2w=0;
            
            for(int i=0;i<number;i++)
            {
                _console=new GameConsole(_controller1,_controller2);
               int t=0;
                if(i%2==0)
                {
                    _console->player1()->setPosition(1600, 500);
                    _console->player2()->setPosition(300, 500);
                }
                while(!_console->update())
                {
                    t++;
                    if(t==10000)
                        break;
                }
                if (_console->player1()->getHp() > _console->player2()->getHp())
                {
                    p1w++;
                }
                if (_console->player1()->getHp() < _console->player2()->getHp())
                {
                    p2w++;
                }
                for(int j=0;j<400;j++)
                {
                    p1->train();
                }
                float p1r=p1->total();
                CCLOG("i:%d",i);
                CCLOG("P1r:%f",p1r);
                CCLOG("P1w:%d",p1w);
                CCLOG("p2w:%d",p2w);
                p1->save(p1r,i);
                delete _console;
            }
        }
    }
    std::string getNewControllerName()
    {
        return _botName;
    }
private:
    int totalgame;
    int p1w;
    int p2w;
    GameController* getConByString(std::string str)
    {
        if(str=="deep")
        {
            return _newControler;
        }
        if(str=="human")
        {
            return MouseController::create();
        }
        if(str=="trad")
        {
            return new OldController(boost::python::object());
        }
    }
    Comander()
    :_botName("newsunho"),
    _isGame(false),
    _controller1(nullptr),
    _controller2(nullptr)
    {
        _newControler=nullptr;
        totalgame=0;
        p1w=0;
        _wins.push_back(0);
        _wins.push_back(0);
        _wins.push_back(0);
        p2w=0;
    };
    ~Comander()
    {
        
    }
    Comander(const Comander& other);
    NewController* _newControler;
    std::string _botName;
    
    bool _isGame;
    std::vector<int> _wins;
    int _p1r;
    int _i;
    std::string _state;
    int _max;
    std::string _con1;
    std::string _con2;
    GameController* _controller1;
    GameController* _controller2;
    GameConsole* _console;
    static Comander* _instance;
};


#endif /* Commander_hpp */
