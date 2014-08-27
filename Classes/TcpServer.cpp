//
//  TcpServer.cpp
//  cocos2dxTest
//
//  Created by othink on 14-8-14.
//
//

#include "TcpServer.h"

TcpServer::TcpServer(int listenPort){
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family=AF_INET;
    localAddr.sin_port=htons(listenPort);
    localAddr.sin_addr.s_addr=htonl(INADDR_ANY);
}

TcpServer::~TcpServer(){
    close(remoteSo);
    close(localSo);
    std::cout<<"TCP Service Closed"<<std::endl;
}

bool TcpServer::iniServer(int instenCount){
    if((localSo=socket(AF_INET, SOCK_STREAM, 0))<0){
        std::cout<<"socket fail."<<std::endl;
        return false;
    }else{
        if (bind(localSo, (sockaddr *)&localAddr, sizeof(localAddr))<0) {
            std::cout<<"bind fail."<<std::endl;
            return false;
        }else{
            std::cout<<"bind success."<<std::endl;
            if (listen(localSo, instenCount)<0) {
                std::cout<<"listen fail."<<std::endl;
                return false;
            }else{
                std::cout<<"Server init success."<<std::endl;
                return true;
            }
        }
    }
}

bool TcpServer::isAccept(){
    unsigned int remoteaddrlen=sizeof(remoteAddr);
    if ((remoteSo=accept(localSo, (sockaddr *)&remoteAddr, &remoteaddrlen))<0) {
        std::cout<<"accept fail."<<std::endl;
        return false;
    }else{
        std::cout<<"Client(IP:"<<inet_ntoa(remoteAddr.sin_addr)<<") connected."<<std::endl;
        return true;
    }
}

bool TcpServer::isConnect(){
    unsigned int remoteaddrlen=sizeof(remoteAddr);
    memset(&remoteAddr, 0, remoteaddrlen);
    remoteAddr.sin_family=AF_INET;
    remoteAddr.sin_port=htons(54321);
    remoteAddr.sin_addr.s_addr=inet_addr("192.168.1.100");
    int sock=socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sock, (sockaddr *)&remoteAddr, remoteaddrlen)<0) {
        std::cout<<"connect fail."<<std::endl;
        return false;
    }else{
        remoteSo=sock;
        std::cout<<"connect success:"<<inet_ntoa(remoteAddr.sin_addr)<<std::endl;
        return true;
    }
}

int TcpServer::recvMsg(char* buffer,unsigned const int len){
    int recvMsgSize=recv(remoteSo,buffer,len,0);
    return recvMsgSize;
}

int TcpServer::sendMsg(char* msg,unsigned const int len){
    int re=send(remoteSo, msg, len, 0);
    return re;
}