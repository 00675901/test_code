//
//  TcpServer.h
//

#ifndef __TcpServer__
#define __TcpServer__

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "GUtils.h"
#include "PackDefine.h"

class TcpServer{
private:
    int localSo;
    sockaddr_in localAddr;
    sockaddr_in remoteAddr;
    pthread_mutex_t tcpMutex;
public:
    TcpServer(int listenPort);
    ~TcpServer();
    int iniServer(int instenCount);
    int isAccept();
    int isAccept(sockaddr_in* remoteAD);
    int isConnect(const char* addr,int rematePort);
    int isConnect(int addr,int rematePort);
    
    long sendData(int remoteSo,char* msg);
    long recvData(int remoteSo,char* buffer);
    
    long sendData(int remoteSo,GNPacket msg);
    long recvData(int remoteSo,GNPacket* buffer);
};
#endif /* defined(__TcpServer__) */


