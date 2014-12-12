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
#include "TcpServer.h"
#include "UdpServer.h"

USING_NS_CC;

#define GCOPC_SIP ((char *)"si01")
#define GCOPC_SSNAME ((char *)"sn01")
#define GCOPC_SCNAME ((char *)"sn02")

class GRServer{
public:
    GRServer(void);
    ~GRServer(void);
};

#endif /* defined(__TestCocos2dx__GRServer__) */
