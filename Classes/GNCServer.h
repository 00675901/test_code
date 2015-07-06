//
//  GRCServer.h
//  TestCocos2dx
//  Created by othink on 14-9-29.
//

#ifndef __TestCocos2dx__GRCServer__
#define __TestCocos2dx__GRCServer__

#include "GNServer.h"

USING_NS_CC;

#define SERVER_STOP 0
#define SERVER_C_RUN 1
#define SERVER_C_PAUSE 2

class GNCServer:public GNServer{
private:
    pthread_mutex_t gncMut;
    pthread_t tidSearchServer;
    pthread_t tidRecvServer;
    pthread_t tidListenNetService;
    map<int,int> serverListStatus;
    map<int,string> serverList;
    deque<string> *msglist;
    map<int,unsigned int> *romateFD;
    
    GNCServer(void);
    ~GNCServer(void);
public:
    static GNCServer* shareInstance();
    
    void startSearchService();
    void pauseSearchService();
    void resumeSearchService();
    void stopSearchService();
    static void* searchServer(void* obj);
    static void* recvServerList(void* obj);

    //service function
    void startConnectService(int addr);
    void stopConnectService();
    
};

#endif /* defined(__TestCocos2dx__GRCServer__) */
