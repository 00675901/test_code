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
#include "cocos2d.h"
#include "cocos-ext.h"
#include "TcpServer.h"
#include "UdpServer.h"

USING_NS_CC;

#define GCOPC_SIP ((char *)"si01")
#define GCOPC_SSNAME ((char *)"sn01")
#define GCOPC_SCNAME ((char *)"sn02")

class GRServer{ 
public:
    unsigned int localIP;
    const char* localName;
    map<int,unsigned int> romateFDIP;
    map<int,string> romateFDName;
    deque<string> loglist;
    
    unsigned int getLocalIP();
    const char* getLocalName();
    map<int,unsigned int> getRomateFDIP();
    map<int,string> getRomateFDName();
    deque<string> getLoglist();
    
    GRServer(void);
    ~GRServer(void);
};

#endif /* defined(__TestCocos2dx__GRServer__) */
