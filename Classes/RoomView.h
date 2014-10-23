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
#include "GRSServer.h"
#include "GRCServer.h"

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

#define RCREATE_FUNC(__TYPE__) \
static __TYPE__* create(int maxl) { \
    __TYPE__ *pRet = new __TYPE__(maxl); \
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
    int maxLinsten;
    int tcpsSocket;
    CCLayerColor* clientLayer;
    CCLayerColor* msgLayer;
    GRSServer* grs;
    GRCServer* gcs;
public:
    static CCScene* scene(int maxl);
    RoomView(int maxl);
    ~RoomView();
    bool init();
    set<int> clientFD;
    deque<string> msglist;
    RCREATE_FUNC(RoomView);
    void closeView();
    void updateRoom();
    void updateMsglist();
};

#endif /* defined(__TestCocos2dx__RoomView__) */
