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
    int remoteSo;
    sockaddr_in localAddr;
    sockaddr_in remoteAddr;
public:
    TcpServer(int listenPort);
    ~TcpServer();
    bool iniServer(int instenCount);
    bool isAccept();
    bool isConnect();
    sockaddr_in* getRemoteRecAddr();
    int sendMsg(char* msg,unsigned const int len);
    int recvMsg(char* buff,unsigned const int len);
};
#endif /* defined(__cocos2dxTest__TcpServer__) */


