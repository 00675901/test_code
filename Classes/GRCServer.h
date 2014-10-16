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
#include <sys/select.h>
#include <errno.h>
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
    pthread_mutex_t mut;
    pthread_t sendudp;
    pthread_t recvudp;
    pthread_t listentcp;
    UdpServer *udps;
    TcpServer *tcps;
    set<int> *roomFD;
    set<string> roomAddr;
    deque<string> *roomName;
    typedef pair<string, string> mapcom;
    fd_set rfdset;
public:
    GRCServer(set<int>* rfd,deque<string>* rn);
    ~GRCServer();
    bool init();
    static void* findRoom(void* obj);
    static void* recvRoom(void* obj);
    static void* listenRoomStatus(void* obj);
};

#endif /* defined(__TestCocos2dx__GRCServer__) */
