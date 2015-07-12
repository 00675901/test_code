//
//  UDPServer.cpp
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
        if (::bind(localSo, (sockaddr *)&localAddr, sizeof(localAddr))<0) {
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

long UdpServer::sendMsg(const char* msg){
    long se;
    if (isBroad) {
        long len=strlen(msg);
        se=sendto(localSo,msg,len,0,(sockaddr *)&remoteBroAddr,sizeof(remoteBroAddr));
    }else{
        se=-1;
    }
    if (se<0) {
        printf("UdpServer_send_fail\n");
    }
    return se;
}

long UdpServer::sendMsg(const char* addr,const char* msg){
    if (!isBroad) {
        remoteBroAddr.sin_addr.s_addr=inet_addr(addr);
    }
    long len=strlen(msg);
    long se=sendto(localSo,msg,len,0,(sockaddr *)&remoteBroAddr,sizeof(remoteBroAddr));
    if (se<0) {
        printf("UdpServer_send_fail\n");
    }
    return se;
}

long UdpServer::recvMsg(char* buff,unsigned const int len){
    long relen=sizeof(remoteRecAddr);
    long recvMsgSize=recvfrom(localSo,buff,len,0,(sockaddr *)&remoteRecAddr,(socklen_t*)&relen);
    return recvMsgSize;
}

long UdpServer::recvMsg(char* buff,unsigned const int len,sockaddr_in* remoteRecAD){
    long relen=sizeof(*remoteRecAD);
    long recvMsgSize=recvfrom(localSo,buff,len,0,(sockaddr *)remoteRecAD,(socklen_t*)&relen);
    return recvMsgSize;
}