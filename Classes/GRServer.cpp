//
//  GRServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14/11/10.
//
//

#include "GRServer.h"

GRServer::GRServer(void){
    printf("GRServer BEGIN\n");
}
GRServer::~GRServer(void){
    printf("GRServer END\n");
}

unsigned int GRServer::getLocalIP(){
    return localIP;
}
const char* GRServer::getLocalName(){
    return localName;
}
map<int,unsigned int> GRServer::getRomateFDIP(){
    return romateFDIP;
}
map<int,string> GRServer::getRomateFDName(){
    return romateFDName;
}
deque<string> GRServer::getLoglist(){
    return loglist;
}