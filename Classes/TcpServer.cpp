//
//  TcpServer.cpp
//  cocos2dxTest
//
//  Created by othink on 14-8-14.
//  Server:Socket->bind->listen->accept
//  Client:Socket->connect
//
//

#include "TcpServer.h"

TcpServer::TcpServer(int listenPort){
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family=AF_INET;
    localAddr.sin_port=htons(listenPort);
    localAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    std::cout<<"TCP Service Begin"<<std::endl;
}

TcpServer::~TcpServer(){
    close(localSo);
    std::cout<<"TCP Service Closed"<<std::endl;
}

int TcpServer::iniServer(int instenCount){
    if((localSo=socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket fail:");
        return -1;
    }else{
        int on=1;
        if (setsockopt(localSo, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))<0) {
            perror("set fail:");
            return -1;
        }else{
            if (instenCount>0) {
                if (bind(localSo, (sockaddr *)&localAddr, sizeof(localAddr))<0) {
                    perror("bind fail:");
                    return -1;
                }else{
                    if (listen(localSo, instenCount)<0) {
                        perror("listen fail:");
                        return -1;
                    }else{
                        std::cout<<"TCP Server Init Success."<<std::endl;
                        return localSo;
                    }
                }
            }else{
                std::cout<<"TCP Client Init Success."<<std::endl;
                return localSo;
            }
        }
    }
}

int TcpServer::isAccept(){
    unsigned int remoteaddrlen=sizeof(remoteAddr);
    int remoteSo;
    if ((remoteSo=accept(localSo, (sockaddr *)&remoteAddr, &remoteaddrlen))<0) {
        std::cout<<"accept fail."<<std::endl;
    }else{
        std::cout<<"Client(IP:"<<inet_ntoa(remoteAddr.sin_addr)<<") connected."<<std::endl;
    }
    return remoteSo;
}

int TcpServer::isAccept(sockaddr_in* remoteAD){
    unsigned int remoteaddrlen=sizeof(*remoteAD);
    int remoteSo;
    if ((remoteSo=accept(localSo, (sockaddr *)remoteAD, &remoteaddrlen))<0) {
        std::cout<<"accept fail."<<std::endl;
    }else{
        std::cout<<"Client(IP:"<<inet_ntoa(remoteAD->sin_addr)<<") connected."<<std::endl;
    }
    return remoteSo;
}

int TcpServer::isConnect(const char* addr,int rematePort){
    return isConnect(inet_addr(addr), rematePort) ;
}

int TcpServer::isConnect(int addr,int rematePort){
    unsigned int remoteaddrlen=sizeof(remoteAddr);
    memset(&remoteAddr, 0, remoteaddrlen);
    remoteAddr.sin_family=AF_INET;
    remoteAddr.sin_port=htons(rematePort);
    remoteAddr.sin_addr.s_addr=addr;
    int remoteSo=socket(AF_INET, SOCK_STREAM, 0);
    if (connect(remoteSo, (sockaddr *)&remoteAddr, remoteaddrlen)<0) {
        perror("connect fail:");
    }else{
        std::cout<<"connect success:"<<inet_ntoa(remoteAddr.sin_addr)<<std::endl;
    }
    return remoteSo;
}

int TcpServer::recvData(int remoteSo,char* buffer){
    int len=strlen(buffer);
    int re=recv(remoteSo,buffer,len,0);
    return re;
}

int TcpServer::sendData(int remoteSo,char* msg){
    int len=strlen(msg);
    int re=send(remoteSo, msg, len, 0);
    return re;
}