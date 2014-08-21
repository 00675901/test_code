//
//  UDPServer.cpp
//  cocos2dxTest
//
//  Created by othink on 14-8-15.
//
//

#include "UdpServer.h"
UdpServer::UdpServer(int listenPort,int remotePort){
    if((localSo=socket(AF_INET, SOCK_DGRAM, 0))<0){
        std::cout<<"socket udp fail"<<std::endl;
    }else{
        std::cout<<"socket upd success."<<std::endl;
        //设置该套接字为广播类型
        const int bOpt = 1;
        int sets=setsockopt(localSo, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));
        std::cout<<"set:"<<sets<<std::endl;
        //本机地址
        memset(&localAddr, 0, sizeof(localAddr));
        localAddr.sin_family=AF_INET;
        localAddr.sin_addr.s_addr=htonl(INADDR_ANY);
        localAddr.sin_port=htons(listenPort);
        //广播地址
        memset(&remoteAddr, 0, sizeof(remoteAddr));
        remoteAddr.sin_family=AF_INET;
        remoteAddr.sin_addr.s_addr=htonl(INADDR_BROADCAST);
        remoteAddr.sin_port=htons(remotePort);
    }
}

int UdpServer::sendMsg(char* msg,unsigned const int len){
    int se=sendto(localSo,msg,len,0,(sockaddr *)&remoteAddr,sizeof(remoteAddr));
    return se;
}

int UdpServer::recvMsg(char* buff,unsigned const int len){
    if (bind(localSo, (sockaddr *)&localAddr, sizeof(localAddr))<0) {
        std::cout<<"bind upd fail."<<std::endl;
        return -1;
    }else{
        int recvMsgSize=recvfrom(localSo, buff,len,0,(sockaddr *)&remoteAddr,(socklen_t*)&len);
        return recvMsgSize;
    }
}

void UdpServer::closeConnect(){
    close(localSo);
    close(remoteSo);
}