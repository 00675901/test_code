//
//  RoomList.h
//  TestCocos2dx
//
//  Created by GgYyer on 14-8-17.
//
//

#ifndef __TestCocos2dx__RoomList__
#define __TestCocos2dx__RoomList__

#include <iostream>
#include <vector>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "TcpServer.h"
#include "UdpServer.h"
#include "TestSer.h"
#include "pthread.h"

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

class RoomList : public cocos2d::CCLayer
{
    pthread_mutex_t* gmut;
    vector<string> gtpool;
private:
    CCLayerColor *listView;
    CCControlButton *createBtn;
    CCControlButton *joinBtn;
    TcpServer *tcps;

    pthread_t mainID;

public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    // implement the "static node()" method manually
    CREATE_FUNC(RoomList);
    void createRoom();
    void joinRoom();
    static void* listenRoom(void* obj);
    void recvClient();
    static void* recvClientService(void* obj);
    void createListView();
    void closeListView();
    
    void testPool();
    void testPthread();

};

#endif /* defined(__TestCocos2dx__RoomList__) */
