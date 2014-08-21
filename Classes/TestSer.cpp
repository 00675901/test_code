//
//  TestSer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-19.
//
//

#include "TestSer.h"

TestServer::TestServer(int listen_port)
{
    if ( (listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ) {
        throw "socket() failed";
    }
    
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(listen_port);
    
    if ( bind(listenSock, (sockaddr*)&servAddr, sizeof(servAddr)) < 0 ) {
        throw "bind() failed";
    }
    
    if ( listen(listenSock, 10) < 0 ) {
        throw "listen() failed";
    }
}

bool TestServer::isAccept()
{
    unsigned int clntAddrLen = sizeof(clntAddr);
    
    if ( (communicationSock = accept(listenSock, (sockaddr*)&clntAddr, &clntAddrLen)) < 0 ) {
        return false;
    } else {
        std::cout << "Client(IP: " << inet_ntoa(clntAddr.sin_addr) << ") connected.\n";
        return true;
    }
}

void TestServer::handleEcho()
{
    char buf[4];
    int recvMsgSize=atoi(buf);
    char buffer[recvMsgSize];
    bool goon = true;
    
    
    std::string contents;
    while ((recvMsgSize = recv(communicationSock, buffer, recvMsgSize, 0))>0) {
            contents.append(buffer,recvMsgSize);
            std::cout<<"count:"<<contents.length()<<std::endl;
            std::cout<<"recv success:"<<contents<<"("<<recvMsgSize<<")."<<std::endl;
//            if ( send(communicationSock, buffer, recvMsgSize, 0) != recvMsgSize ) {
//                throw "send() failed";
//            }
    }
    printf("test:%s(%d)\n",contents.c_str(),(int)contents.length());
//    close(communicationSock);
}