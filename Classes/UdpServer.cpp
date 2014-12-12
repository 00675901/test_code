//
//  UDPServer.cpp
//  cocos2dxTest
//
//  Created by othink on 14-8-15.
//
//

#include "UdpServer.h"
UdpServer::UdpServer(int listenPort,int remotePort,bool isBro){
    //本机地址
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family=AF_INET;
    localAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    localAddr.sin_port=htons(listenPort);
    //广播地址
    memset(&remoteBroAddr, 0, sizeof(remoteBroAddr));
    remoteBroAddr.sin_family=AF_INET;
    remoteBroAddr.sin_port=htons(remotePort);
    isBroad=isBro;
    std::cout<<"UDP Service Begin"<<std::endl;
}
UdpServer::~UdpServer(){
    close(localSo);
    std::cout<<"UDP Service Closed"<<std::endl;
}
bool UdpServer::iniServer(){
    if((localSo=socket(AF_INET, SOCK_DGRAM, 0))<0){
        perror("socket udp fail:");
        return false;
    }else{
        if (bind(localSo, (sockaddr *)&localAddr, sizeof(localAddr))<0) {
            perror("bind udp fail:");
            return false;
        }else{
            if (isBroad) {
                //设置该套接字为广播类型并且设置远程地址为广播地址
                remoteBroAddr.sin_addr.s_addr=htonl(INADDR_BROADCAST);
                const int bOpt = 1;
                int sets;
                if ((sets=setsockopt(localSo, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt)))<0) {
                    perror("udp set fail:");
                    return false;
                }else{
                    std::cout<<"UDP Broadcast Init Success:"<<sets<<std::endl;
                    return true;
                }
            }
            std::cout<<"UDP Init Success:"<<std::endl;
            return true;
        }
    }
}

int UdpServer::sendMsg(const char* msg){
    int se;
    if (isBroad) {
        int len=strlen(msg);
        se=sendto(localSo,msg,len,0,(sockaddr *)&remoteBroAddr,sizeof(remoteBroAddr));
    }else{
        se=-1;
    }
    return se;
}

int UdpServer::sendMsg(const char* addr,const char* msg){
    if (!isBroad) {
        remoteBroAddr.sin_addr.s_addr=inet_addr(addr);
    }
    int len=strlen(msg);
    int se=sendto(localSo,msg,len,0,(sockaddr *)&remoteBroAddr,sizeof(remoteBroAddr));
    return se;
}

int UdpServer::recvMsg(char* buff,unsigned const int len){
    int relen=sizeof(remoteRecAddr);
    int recvMsgSize=recvfrom(localSo,buff,len,0,(sockaddr *)&remoteRecAddr,(socklen_t*)&relen);
    return recvMsgSize;
}

int UdpServer::recvMsg(char* buff,unsigned const int len,sockaddr_in* remoteRecAD){
    int relen=sizeof(*remoteRecAD);
    int recvMsgSize=recvfrom(localSo,buff,len,0,(sockaddr *)remoteRecAD,(socklen_t*)&relen);
    return recvMsgSize;
}