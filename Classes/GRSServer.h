//
//  GRSServer.h
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#ifndef __TestCocos2dx__GRSServer__
#define __TestCocos2dx__GRSServer__

#include <stdio.h>

#include <sys/select.h>
#include "cocos2d.h"
#include "TcpServer.h"
#include "UdpServer.h"
#include "pthread.h"
#include "GSNotificationPool.h"
#include <errno.h>
#include "GUtils.h"

USING_NS_CC;
using namespace std;

class GRSServer{
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
public:
    GRSServer(int maxl);
    ~GRSServer();
    bool init();
    static void* listenRoomService(void* obj);
    static void* sendRoomService(void* obj);
};

#endif /* defined(__TestCocos2dx__GRSServer__) */
