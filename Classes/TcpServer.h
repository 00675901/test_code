//
//  TcpServer.h
//  cocos2dxTest
//
//  Created by othink on 14-8-14.
//
//

#ifndef __cocos2dxTest__TcpServer__
#define __cocos2dxTest__TcpServer__

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "GUtils.h"

using namespace std;

class TcpServer{
private:
    int localSo;
    sockaddr_in localAddr;
    sockaddr_in remoteAddr;
public:
    TcpServer(int listenPort);
    ~TcpServer();
    int iniServer(int instenCount);
    int isAccept();
    int isConnect(const char* addr,int rematePort);
    sockaddr_in* getRemoteRecAddr();
    int sendMsg(int remoteSo,char* msg,unsigned const int len);
    int recvMsg(int remoteSo,char* buff,unsigned const int len);
};
#endif /* defined(__cocos2dxTest__TcpServer__) */


