//
//  MainScene.hpp
//  AI
//
//  Created by sunho kim on 04/11/2017.
//

#ifndef MainScene_hpp
#define MainScene_hpp

#include <stdio.h>
#include "cocos2d.h"

class MainScene : public cocos2d::Scene
{
public:
    virtual bool init();
    bool gotone;
    CREATE_FUNC(MainScene);
};
#endif /* MainScene_hpp */
