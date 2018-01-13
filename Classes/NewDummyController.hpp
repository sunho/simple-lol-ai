//
//  NewDummyController.hpp
//  AI
//
//  Created by sunho kim on 2017/12/05.
//

#ifndef NewDummyController_hpp
#define NewDummyController_hpp

#include <stdio.h>
#include <stdio.h>
#include "GameController.hpp"

class NewDummyController: public GameController
{
    public:
    NewDummyController(){};
    virtual ~NewDummyController(){};
    void doAction(int action);
    void middleQ();
    void avoidE();
    void backE();
    void leftQ();
    void rightQ();
    void frontE();
    virtual std::string oname(){return "new";};
    virtual void notify(int,std::string str){};
    virtual void updates(int timestep){};
};
#endif /* NewDummyController_hpp */
