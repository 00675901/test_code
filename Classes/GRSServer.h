//
//  GRSServer.h
//  TestCocos2dx
//  Created by othink on 14-9-29.
//

#ifndef __TestCocos2dx__GRSServer__
#define __TestCocos2dx__GRSServer__

#include <stdio.h>

#include <sys/select.h>
#include <errno.h>
#include "GRServer.h"
#include "pthread.h"
#include "GSNotificationPool.h"
#include "GUtils.h"

USING_NS_CC;

using namespace std;

class GRSServer:public GRServer{
private:
    pthread_t tidudp;
    pthread_t tidtcp;
    pthread_mutex_t mut;
    TcpServer *tcps;
    int maxLinsten;
    int tcpsSocket;
    UdpServer *udps;
    fd_set rfdset;
    GRSServer(void);
    ~GRSServer(void);
public:
    static GRSServer* shareInstance();
    //service
    void startSendRoomService();
    void stopSendRoomService();
    static void* sendRoomService(void* obj);
    //service
    void startListenRoomService(int maxl,const char* uname);
    void stopListenRoomService();
    static void* listenRoomService(void* obj);
    
    void sendMsgToAll(char* msg);
};

#endif /* defined(__TestCocos2dx__GRSServer__) */
