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
#include "TcpServer.h"
#include "UdpServer.h"
#include "pthread.h"
#include "GSNotificationPool.h"

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

class RoomView : public CCLayerColor{
private:
    pthread_t tidudp;
    pthread_t tidtcp;
    TcpServer *tcps;
    UdpServer *udps;
public:
    RoomView();
    ~RoomView();
    virtual bool init();
    CREATE_FUNC(RoomView);
    void closeView();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void roomServer();
    static void* sendRoomService(void* obj);
    static void* listenRoomService(void* obj);
};

#endif /* defined(__TestCocos2dx__RoomView__) */
