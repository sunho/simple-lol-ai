//
//  OldController.hpp
//  AI
//
//  Created by sunho kim on 2017/11/10.
//

#ifndef OldController_hpp
#define OldController_hpp

#include <stdio.h>
#include "GameController.hpp"
#include "PlayerListener.hpp"
#include "NewController.hpp"
class OldController : public GameController
{
public:
    OldController(boost::python::object module);
    void notify(int id,std::string str);
    void setType(int type);
    virtual void updates(int timestep);
    float end();
    void calculate();
    virtual std::string oname(){return "old";};
    int getAction();
    std::vector<float> getState();
    void doAction(int a);
    float killAngle(Player* player);
    int backward();
private:
    int ds;
    int _avoiding;
    int _avoidingDir;
    int _type=0;
    int _tick;
    int _step;
    int _recentaction;
    std::vector<float> _recentstate;
    float _totalreward;
    int _doing;
    bool _asdf;
    float _reward;
    boost::python::object _module;
};
#endif /* OldController_hpp */
