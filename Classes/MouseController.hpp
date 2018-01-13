//
//  MouseController.hpp
//  AI
//
//  Created by sunho kim on 2017/11/09.
//

#ifndef MouseController_hpp
#define MouseController_hpp

#include <stdio.h>
#include "GameController.hpp"
#include <cocos2d.h>

class MouseController : public GameController, public cocos2d::Sprite
{
public:
    virtual bool init();
    ~MouseController(){};
    virtual std::string oname(){return "mouse";};
    virtual void notify(int id,std::string str){};
    CREATE_FUNC(MouseController);
    virtual void updates(int timestep);
private:
    float _mouseX;
    float _mouseY;
};
#endif /* MouseController_hpp */
