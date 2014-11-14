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
#include "cocos2d.h"
#include "cocos-ext.h"
#include "GRServer.h"
#include "TcpServer.h"
#include "UdpServer.h"
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
    map<int,unsigned int> *clientFD;
    deque<string> *msglist;
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
    void startListenRoomService(int maxl,map<int,unsigned int>* cf,deque<string>* ml);
    void stopListenRoomService();
    static void* listenRoomService(void* obj);
    
    void sendMsgToAll(char* msg);
};

#endif /* defined(__TestCocos2dx__GRSServer__) */
