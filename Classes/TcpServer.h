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
    int isAccept(sockaddr_in* remoteAD);
    int isConnect(const char* addr,int rematePort);
    int isConnect(int addr,int rematePort);
    int sendData(int remoteSo,char* msg);
    int recvData(int remoteSo,char* buffer);
};
#endif /* defined(__cocos2dxTest__TcpServer__) */


