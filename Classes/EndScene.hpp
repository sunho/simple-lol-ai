//
//  EndScene.hpp
//  AI
//
//  Created by sunho kim on 2017/12/20.
//

#ifndef EndScene_hpp
#define EndScene_hpp

#include <stdio.h>
#include "Commander.hpp"

class EndScene : public cocos2d::Scene
{
public:
    virtual bool init(std::string con1,std::string con2,int winlost);
    static EndScene* create(std::string con1,std::string con2,int winlost);
private:

};
#endif /* EndScene_hpp */
