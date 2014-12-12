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

int TcpServer::sendData(int fd,GCData* pack){
    if (pack->opcode.empty()) {
        pack->opcode="0000";
    }
    int oplen=pack->opcode.length();
    if (oplen==4) {
        int len=pack->data.length()+4;
        char sh[5];
        sprintf(sh,"%04d",len);
        int isl=len+4;
        char sendchar[isl];
        sprintf(sendchar,"%s%s%s",sh,pack->opcode.c_str(),pack->data.c_str());
        int re=send(fd,sendchar,isl,MSG_WAITALL);
        printf("sendCC:%s\nsend:%s\nopc:%s\nsize:%d\n",sh,pack->data.c_str(),pack->opcode.c_str(),re);
        return re;
    }else{
        printf("sendData error:opcode Error\n");
        return -1;
    }
}
int TcpServer::recvData(int fd,GCData* pack){
    char bufhead[5];
    int ret=recv(fd, bufhead, 4, MSG_WAITALL);
    int dl=0;
    if (ret==4) {
        bufhead[4]='\0';
        dl=GUtils::ctoi(bufhead);
    }
    if (dl>0) {
        char bufcon[dl];
        ret=recv(fd, bufcon, dl, MSG_WAITALL);
        string tempcon=bufcon;
        pack->opcode=tempcon.substr(0,4);
        pack->data=tempcon.substr(4,dl-4);
    }
    return ret;
}