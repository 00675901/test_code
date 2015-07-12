//
//  RoomlistView.h
//  TestCocos2dx
//
//  Created by othink on 14-10-9.
//
//

#ifndef __TestCocos2dx__RoomlistView__
#define __TestCocos2dx__RoomlistView__

#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "GSNotificationPool.h"
#include "RoomView.h"

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

#define RLCREATE_FUNC(__TYPE__) \
static __TYPE__* create(const char* username) { \
    __TYPE__ *pRet = new __TYPE__(username); \
    if (pRet && pRet->init()){ \
        pRet->autorelease(); \
        return pRet; \
    }else{ \
        delete pRet; \
        pRet = NULL; \
        return NULL; \
    } \
}

class RoomlistView : public CCLayerColor{
private:
    CCLayerColor* roomListLayer;
    const char* uname;
    NetAppCCJSController* gnapp;
public:
    map<unsigned int,string>* roomlist;
    static CCScene* scene(const char* username);
    RoomlistView(const char* username);
    ~RoomlistView();
    RLCREATE_FUNC(RoomlistView);
    virtual bool init();
    void closeView();
    void updateRoomlist();
    void enterRoom(CCControlButton *sender);
};


#endif /* defined(__TestCocos2dx__RoomlistView__) */
