//
//  PlayerListener.hpp
//  AI
//
//  Created by sunho kim on 2017/11/06.
//

#ifndef PlayerListener_hpp
#define PlayerListener_hpp

#include <stdio.h>
#include <string>

static std::string* StringSplit(std::string strTarget, std::string strTok)
{
    int     nCutPos;
    int     nIndex     = 0;
    std::string* strResult = new std::string[20];
    
    while ((nCutPos = strTarget.find_first_of(strTok)) != strTarget.npos)
    {
        if (nCutPos > 0)
        {
            strResult[nIndex++] = strTarget.substr(0, nCutPos);
        }
        strTarget = strTarget.substr(nCutPos+1);
    }
    
    if(strTarget.length() > 0)
    {
        strResult[nIndex++] = strTarget.substr(0, nCutPos);
    }
    
    return strResult;
}
class PlayerListener
{
public:
    virtual void notify(int,std::string)=0;
};
#endif /* PlayerListener_hpp */
