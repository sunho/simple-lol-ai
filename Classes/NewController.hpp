//
//  NewController.hpp
//  AI
//
//  Created by sunho kim on 2017/11/14.
//

#ifndef NewController_hpp
#define NewController_hpp

#include <stdio.h>
#include "GameController.hpp"

class NewController: public GameController
{
public:
    NewController(std::string name,int training);
    NewController(boost::python::object module);
    ~NewController(){};
    void save(float a,int timestep);
    float end();
    std::vector<float> getState();
    int getAction();
    void train();
    void realend();
    void calculate();
    void scoreAppend(float score);
    void middleQ();
    void leftQ();
    void left2Q();
    void rightQ();
    void right2Q();
    void avoidE();
    float total();
    void backE();
    void frontE();
    void doAction(int action);
    float getReward();
    void setName(std::string name);
    virtual std::string oname(){return "new";};
    void saveImg(std::string path);
    virtual void notify(int id,std::string str);
    virtual void updates(int timestep);
    boost::python::object _module;
private:
    int _tick;
    int _step;
    int _avoiding;
    int _avoidingDir;
    int _recentaction;
    std::vector<float> _recentstate;
    float _totalreward;
    int _doing;
    bool _asdf;
    float _reward;
    boost::python::object _agent;
};
#endif /* NewController_hpp */
