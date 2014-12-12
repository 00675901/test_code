//
//  UDPServer.h
//  cocos2dxTest
//
//  Created by othink on 14-8-15.
//
//

#ifndef __cocos2dxTest__UdpServer__
#define __cocos2dxTest__UdpServer__

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

class UdpServer{
private:
    sockaddr_in localAddr;
    sockaddr_in remoteBroAddr;
    sockaddr_in remoteRecAddr;
    bool isBroad;
public:
    int localSo;
    UdpServer(int listenPort,int remotePort,bool isBro);
    ~UdpServer();
    bool iniServer();
    int sendMsg(const char* msg);
    int sendMsg(const char* addr,const char* msg);
    int recvMsg(char* buff,unsigned const int len);
    int recvMsg(char* buff,unsigned const int len,sockaddr_in* remoteRecAD);
};

#endif /* defined(__cocos2dxTest__UdpServer__) */
