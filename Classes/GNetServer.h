//
//  GNetServer.h
//  TestCocos2dx
//
//  Created by othink on 15/1/6.
//
//

#ifndef __TestCocos2dx__GNetServer__
#define __TestCocos2dx__GNetServer__

#include <stdio.h>
#include <sys/select.h>
#include <errno.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "TcpServer.h"
#include "UdpServer.h"
#include "pthread.h"
#include "GSNotificationPool.h"
#include "GUtils.h"

USING_NS_CC;

#define SERVER_STOP 0
#define SERVER_S_RUN 1
#define SERVER_S_PAUSE 2
#define SERVER_C_RUN 3
#define SERVER_C_PAUSE 4
#define SERVER_C_TCP_RUN 5

#define GNOC_GIP ((char *)"si01")
#define GNOC_SIP ((char *)"si02")
#define GNOC_SSNAME ((char *)"sn01")
#define GNOC_SCNAME ((char *)"sn02")

class GNetServer{
private:
    GNetServer(void);
    ~GNetServer(void);
    
    //public system member
    UdpServer *udps;
    TcpServer *tcps;
    int serverStatus;
    pthread_mutex_t mut;
    unsigned int localIP;
    int localFD;
    const char* localName;
    map<int,unsigned int> remoteFDIP;
    map<int,string> remoteFDName;
    deque<string> loglist;
    fd_set rfdset;
    
    //UDP Send localIP Service function
    int maxLinsten;
    int localTcpFD;
    pthread_t tidRoomService;
    pthread_t tidListenRoomService;
    
    //UDP search response
    map<int, string> serverList;
    map<int,int> serverListStatus;
    pthread_t tidSearchServer;
    pthread_t tidRecvServer;
    
    //TCP connect
    pthread_t tidConnectService;
public:
    static GNetServer* shareInstance();
    
    //use member function
    unsigned int getLocalIP();
    const char* getLocalName();
    map<int,unsigned int> getRemoteFDIP();
    map<int,string> getRemoteFDName();
    deque<string> getLoglist();
    
    //UDP Send localIP Service function
    void startResponseService(int maxl,const char* uname);
    void pauseResponseService();
    void resumeResponseService();
    void stopResponseService();
    static void* responseService(void* obj);
    //UDP search response
    void startSearchService();
    void pauseSearchService();
    void resumeSearchService();
    void stopSearchService();
    static void* searchServer(void* obj);
    static void* recvServerList(void* obj);
    //TCP connect
    void startConnectService(int addr);
    void stopConnectService();
    //监听tcp通信
    static void* listenNetService(void* obj);

};

#endif /* defined(__TestCocos2dx__GNetServer__) */
