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
#include <sys/select.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "TcpServer.h"
#include "UdpServer.h"
#include "pthread.h"
#include "GSNotificationPool.h"
#include <errno.h>
#include "GUtils.h"

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
    pthread_t tidudp;
    pthread_t tidtcp;
    pthread_mutex_t mut;
    TcpServer *tcps;
    int maxLinsten;
    int tcpsSocket;
    UdpServer *udps;
    set<int> clientFD;
    fd_set rfdset;
    fd_set wfdset;
    fd_set efdset;
    CCLayerColor *clientLayer;
    CCLayerColor *msgLayer;
    deque<string> msglist;
public:
    RoomView(int maxl);
    ~RoomView();
    bool init();
    RCREATE_FUNC(RoomView);
    void closeView();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void roomServer();
    static void* sendRoomService(void* obj);
    static void* listenRoomService(void* obj);
    void updateRoom(CCObject *param);
    void updateMsglist(CCObject *param);
    void sendMsgToAll(const char* msg);
};

#endif /* defined(__TestCocos2dx__RoomView__) */
