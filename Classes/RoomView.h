//
//  RoomView.h
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#ifndef __TestCocos2dx__RoomView__
#define __TestCocos2dx__RoomView__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "GSNotificationPool.h"
#include <errno.h>
#include "GUtils.h"
#include "GNSServer.h"
#include "GNCServer.h"

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

#define RCREATE_FUNC(__TYPE__) \
static __TYPE__* create(int maxl,bool isServer,const char* username) { \
    __TYPE__ *pRet = new __TYPE__(maxl,isServer,username); \
    if (pRet && pRet->init()){ \
        pRet->autorelease(); \
        return pRet; \
    }else{ \
        delete pRet; \
        pRet = NULL; \
        return NULL; \
    } \
}

class RoomView : public CCLayerColor{
private:
    bool isSer;
    const char* uname;
    int maxLinsten;
    int localTcpFD;
    CCLayerColor* clientLayer;
    CCLayerColor* msgLayer;
    GNSServer* grs;
    GNCServer* gcs;
public:
    static CCScene* scene(int maxl,bool isServer,const char* username);
    RoomView(int maxl,bool isServer,const char* username);
    ~RoomView();
    bool init();
    RCREATE_FUNC(RoomView);
    void closeView();
    void updateRoom();
    void updateMsglist();
};

#endif /* defined(__TestCocos2dx__RoomView__) */
