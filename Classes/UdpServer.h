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
    int localSo;
    int remoteSo;
    sockaddr_in localAddr;
    sockaddr_in remoteAddr;
public:
    UdpServer(int listenPort,int remotePort);
    int sendMsg(char* msg,unsigned const int len);
    int recvMsg(char* buff,unsigned const int len);
    void closeConnect();
};

#endif /* defined(__cocos2dxTest__UdpServer__) */
