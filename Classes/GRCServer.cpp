//
//  GRCServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRCServer.h"

GRCServer::GRCServer(map<string,string>* ra){
    roomAddr=ra;
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
        pthread_create(&recvudp,NULL,GRCServer::recvRoom,this);
    }
    return true;
}

void* GRCServer::findRoom(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    UdpServer *temp=(UdpServer *)obj;
    while (true) {
        pthread_testcancel();
        char s[]="1";
        temp->sendMsg(s, 1);
        sleep(1);
    }
    return NULL;
}

void* GRCServer::recvRoom(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GRCServer* tempgr=(GRCServer*)obj;
    UdpServer* tempudps=tempgr->udps;
    map<string,string>* ras=tempgr->roomAddr;
    while (true) {
        pthread_testcancel();
        int res;
        char tbuffer[9];
        if ((res=tempudps->recvMsg(tbuffer, 9))>0) {
            string temps=tbuffer;
            cout<<"net udp msg:"<<temps<<endl;
            string ss=GUtils::cptos(inet_ntoa(tempudps->getRemoteRecAddr()->sin_addr));
            cout<<"retome IP:"<<ss<<endl;
            ras->insert(mapcom(ss,temps));
            GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
        }
    }
    return NULL;
}