//
//  TestSer.h
//  TestCocos2dx
//
//  Created by othink on 14-8-19.
//
//

#ifndef __TestCocos2dx__TestSer__
#define __TestCocos2dx__TestSer__

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "GUtils.h"

class TestServer{
private:
    int listenSock;
    int communicationSock;
    sockaddr_in servAddr;
    sockaddr_in clntAddr;
public:
    TestServer(int listen_port);
    bool isAccept();
    void handleEcho();
};


#endif /* defined(__TestCocos2dx__TestSer__) */
