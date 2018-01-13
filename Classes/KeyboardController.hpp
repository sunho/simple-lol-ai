//
//  KeyboardController.hpp
//  AI
//
//  Created by sunho kim on 2017/11/09.
//

#ifndef KeyboardController_hpp
#define KeyboardController_hpp

#include <stdio.h>
#include "GameController.hpp"
#include <cocos2d.h>

class KeyboardController : public GameController, public cocos2d::Sprite
{
public:
    virtual bool init();
    ~KeyboardController(){};
    virtual std::string oname(){return "keyboard";};
    virtual void notify(int id,std::string std){};
    CREATE_FUNC(KeyboardController);
    virtual void updates(int timestep);
};
#endif /* KeyboardController_hpp */

