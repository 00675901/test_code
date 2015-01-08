//
//  GRServer.h
//  TestCocos2dx
//
//  Created by othink on 14/11/10.
//
//

#ifndef __TestCocos2dx__GRServer__
#define __TestCocos2dx__GRServer__

#include <stdio.h> 
#include <errno.h>
#include <sys/select.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "TcpServer.h"
#include "UdpServer.h"
#include "pthread.h"
#include "GSNotificationPool.h"
#include "GUtils.h"

USING_NS_CC;

#define GNOC_GIP ((char *)"si01")
#define GNOC_SIP ((char *)"si02")
#define GNOC_SSNAME ((char *)"sn01")
#define GNOC_SCNAME ((char *)"sn02")

class GNServer{
private:
    fd_set rfdset;
protected:
    pthread_mutex_t mut;
    UdpServer *udps;
    TcpServer *tcps;
    int serverStatus;
    unsigned int localIP;
    int localTcpFD;
    map<int,unsigned int> remoteFDIP;
    map<int,string> serverList;
public:
    unsigned int getLocalIP();
    map<int,unsigned int> getRemoteFDIP();

    GNServer(void);
    ~GNServer(void);
    static void* listenNetService(void* obj);
};

#endif /* defined(__TestCocos2dx__GRServer__) */
