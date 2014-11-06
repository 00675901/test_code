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

int TcpServer::isConnect(const char* addr,int rematePort){
    unsigned int remoteaddrlen=sizeof(remoteAddr);
    memset(&remoteAddr, 0, remoteaddrlen);
    remoteAddr.sin_family=AF_INET;
    remoteAddr.sin_port=htons(rematePort);
    remoteAddr.sin_addr.s_addr=inet_addr(addr);
    int remoteSo=socket(AF_INET, SOCK_STREAM, 0);
    if (connect(remoteSo, (sockaddr *)&remoteAddr, remoteaddrlen)<0) {
        perror("connect fail:");
//        std::cout<<"connect fail."<<std::endl;
    }else{
        std::cout<<"connect success:"<<inet_ntoa(remoteAddr.sin_addr)<<std::endl;
    }
    return remoteSo;
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
//        std::cout<<"connect fail."<<std::endl;
    }else{
        std::cout<<"connect success:"<<inet_ntoa(remoteAddr.sin_addr)<<std::endl;
    }
    return remoteSo;
}

//int TcpServer::recvMsg(int remoteSo,char* buffer,unsigned const int len){
//    int re=recv(remoteSo,buffer,len,0);
//    return re;
//}

int TcpServer::recvMsg(int remoteSo,char* buffer){
    char bufhead[4];
    int ret=recv(remoteSo, bufhead, 4, MSG_WAITALL);
    int dl=0;
    if (ret==4) {
        dl=GUtils::ctoi(bufhead);
    }
    printf("head:%s=====%d\n",bufhead,dl);
    if (dl>0) {
        ret=recv(remoteSo, buffer, dl, MSG_WAITALL);
    }
    printf("recv content:%s\n",buffer);
    printf("recv content size:====%d\n",ret);
    return ret;
}

//int TcpServer::sendMsg(int remoteSo,char* msg,unsigned const int len){
//    int re=send(remoteSo, msg, len, 0);
//    return re;
//}

int TcpServer::sendMsg(int remoteSo,char* msg){
    const int len=strlen(msg);
    printf("length:%d-----%04d\n",len,len);
    char sh[4];
    sprintf(sh,"%04d",len);
    printf("send head:%s\n",sh);
    int isl=len+4;
    char sendchar[isl];
    sprintf(sendchar,"%s%s",sh,msg);
    printf("send content:%s  size:%d\n",sendchar,isl);
    int re=send(remoteSo,sendchar,isl,MSG_WAITALL);
    printf("send result:%d\n",re);
    return re;
}

sockaddr_in* TcpServer::getRemoteRecAddr(){
    return &remoteAddr;
}