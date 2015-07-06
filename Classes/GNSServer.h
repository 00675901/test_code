//
//  GRSServer.h
//  TestCocos2dx
//  Created by othink on 14-9-29.
//

#ifndef __TestCocos2dx__GRSServer__
#define __TestCocos2dx__GRSServer__

#include "GNServer.h"

USING_NS_CC;

#define SERVER_STOP 0
#define SERVER_S_RUN 1
#define SERVER_S_PAUSE 2

class GNSServer:public GNServer{
private:
    pthread_t tidResponseService;
    pthread_t tidListenRoomService;
    int maxLinsten;
    const char* localName;
    GNSServer(void);
    ~GNSServer(void);
public:
    static GNSServer* shareInstance();
    
    void startResponseService(int maxl,const char* uname);
    void pauseResponseService();
    void resumeResponseService();
    void stopResponseService();
    static void* responseService(void* obj);
};

#endif /* defined(__TestCocos2dx__GRSServer__) */
