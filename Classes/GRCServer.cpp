//
//  GRCServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRCServer.h"

GRCServer::GRCServer(){
    cout<<"Client Server service BEGIN"<<endl;
}
GRCServer::~GRCServer(){
    pthread_cancel(sendudp);
    pthread_cancel(recvudp);
    delete udps;
    cout<<"Client Server service END"<<endl;
}
bool GRCServer::init(){
    udps=new UdpServer(50003,50002);
    if (udps->iniServer()) {
        pthread_create(&sendudp,NULL,GRCServer::findRoom,udps);
        pthread_create(&recvudp,NULL,GRCServer::recvRoom,udps);
    }
    return true;
}

void* GRCServer::findRoom(void* obj){
    UdpServer *temp=(UdpServer *)obj;
    while (true) {
        char s[]="1";
        temp->sendMsg(s, strlen(s));
        sleep(1);
    }
    return NULL;
}

void* GRCServer::recvRoom(void* obj){
    UdpServer *temp=(UdpServer *)obj;
    while (true) {
        int res;
        char tbuffer[1];
        if ((res=temp->recvMsg(tbuffer, 1))>0) {
            string temps=tbuffer;
            cout<<"net udp msg:"<<temps<<endl;
        }
    }
    return NULL;
}