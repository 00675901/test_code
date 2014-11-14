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
#include "GRServer.h"
#include "pthread.h"

using namespace std;

class GRCServer:public GRServer{
private:
    pthread_mutex_t mut;
    pthread_t sendudp;
    pthread_t recvudp;
    pthread_t listenRS;
    pthread_t listenRoc;
    UdpServer *udps;
    TcpServer *tcps;
    map<int,string> *roomlistInfo;
    map<int,int> roomStatus;
    fd_set rfdset;
    fd_set trfdset;
    int localFD;
    int serverFD;
    deque<string> *msglist;
    map<int,unsigned int> *romateFD;
    GRCServer(void);
    ~GRCServer(void);
public:
    static GRCServer* shareInstance();
    //service
    void startFindRoomService(map<int, string>* rli);
    void stopFindRoomService();
    static void* findRoom(void* obj);
    static void* recvRoom(void* obj);
    static void* listenRoomStatus(void* obj);
    //service function
    bool initConnectService(int addr);
    void startConnectService(map<int,unsigned int>* cf,deque<string> *ml);
    void stopConnectService();
    static void* listenRoomService(void* obj);
};

#endif /* defined(__TestCocos2dx__GRCServer__) */
