//
//  UDPServer.cpp
//  cocos2dxTest
//
//  Created by othink on 14-8-15.
//
//

#include "UdpServer.h"
UdpServer::UdpServer(int listenPort,int remotePort){
    //本机地址
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family=AF_INET;
    localAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    localAddr.sin_port=htons(listenPort);
    //广播地址
    memset(&remoteBroAddr, 0, sizeof(remoteBroAddr));
    remoteBroAddr.sin_family=AF_INET;
    remoteBroAddr.sin_addr.s_addr=htonl(INADDR_BROADCAST);
    remoteBroAddr.sin_port=htons(remotePort);
    std::cout<<"UDP Service Begin"<<std::endl;
}

UdpServer::~UdpServer(){
    close(localSo);
    close(remoteSo);
    std::cout<<"UDP Service Closed"<<std::endl;
}

bool UdpServer::iniServer(){
    if((localSo=socket(AF_INET, SOCK_DGRAM, 0))<0){
        std::cout<<"socket udp fail"<<std::endl;
        return false;
    }else{
        if (bind(localSo, (sockaddr *)&localAddr, sizeof(localAddr))<0) {
            std::cout<<"bind upd fail."<<std::endl;
            return false;
        }else{
            //设置该套接字为广播类型
            const int bOpt = 1;
            int sets;
            if ((sets=setsockopt(localSo, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt)))<0) {
                std::cout<<"udp set fail"<<std::endl;
                return false;
            }else{
                std::cout<<"UDP Init Success:"<<sets<<std::endl;
                return true;
            }
        }
    }
}

int UdpServer::sendMsg(const char* msg,unsigned const int len){
    int se=sendto(localSo,msg,len,0,(sockaddr *)&remoteBroAddr,sizeof(remoteBroAddr));
    return se;
}

int UdpServer::recvMsg(char* buff,unsigned const int len){
    int relen=sizeof(remoteRecAddr);
    int recvMsgSize=recvfrom(localSo,buff,len,0,(sockaddr *)&remoteRecAddr,(socklen_t*)&relen);
    return recvMsgSize;
}

sockaddr_in* UdpServer::getRemoteRecAddr(){
    return &remoteRecAddr;
}