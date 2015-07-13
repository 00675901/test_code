//
//  TcpServer.cpp
//
//  Server:Socket->bind->listen->accept
//  Client:Socket->connect
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
                if (::bind(localSo, (sockaddr *)&localAddr, sizeof(localAddr))<0) {
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

long TcpServer::recvData(int remoteSo,char* buffer){
    long len=strlen(buffer);
    long re=recv(remoteSo,buffer,len,0);
    if (re<0) {
        printf("TCP_send_fail：%ld\n",re);
    }
    return re;
}

long TcpServer::sendData(int remoteSo,char* msg){
    long len=strlen(msg);
    printf("send content:%s\n",msg);
    long re=send(remoteSo, msg, len, 0);
    if (re<0) {
        printf("TCP_send_fail\n");
    }
    return re;
}

long TcpServer::sendData(int remoteSo,GNPacket* msg){
    int templen=msg->size();
    char sendco[templen];
    msg->serializer(sendco);
    char send[templen+5];
    sprintf(send, "%04d%s",templen,sendco);
    long re=sendData(remoteSo,send);
    return re;
}
long TcpServer::recvData(int remoteSo,GNPacket* buffer){
    char bufhead[5];
    int ret=recv(remoteSo, bufhead, 4, 0);
    if (ret<0) {
        printf("TCP_recv_sise_fail:%d\n",ret);
    }else{
        printf("content size:%d\n",ret);
    }
    int dl=0;
    if (ret==4) {
        bufhead[4]='\0';
        dl=GUtils::ctoi(bufhead);
        printf("parser_content size:%d\n",dl);
    }
    if (dl>0) {
        char bufcon[dl];
        printf("begin recv:%d\n",dl);
        ret=recv(remoteSo, bufcon, dl, 0);
        if (ret<0) {
            printf("TCP_recv_fail:%d\n",ret);
        }else{
            printf("real-content size:%d\n",ret);
        }
        buffer->deserializer(bufcon);
        printf("recv Content Packet:%d,%d,%s,%d,%s\n",buffer->sysCode,buffer->origin,buffer->UUID.c_str(),buffer->NPCode,buffer->data.c_str());
    }
    return ret;
}
