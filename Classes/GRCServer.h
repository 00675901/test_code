//
//  GRCServer.h
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#ifndef __TestCocos2dx__GRCServer__
#define __TestCocos2dx__GRCServer__

#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "GUtils.h"
#include "GSNotificationPool.h"
#include "UdpServer.h"
#include "TcpServer.h"
#include "pthread.h"

using namespace std;

class GRCServer{
private:
    pthread_t sendudp;
    pthread_t recvudp;
    UdpServer *udps;
    TcpServer *tcps;
    deque<int> *roomFD;
    deque<string> *roomName;
    typedef pair<string, string> mapcom;
    set<string> roomAddr;
public:
    GRCServer(deque<int>* rfd,deque<string>* rn);
    ~GRCServer();
    bool init();
    static void* findRoom(void* obj);
    static void* recvRoom(void* obj);
};

#endif /* defined(__TestCocos2dx__GRCServer__) */
