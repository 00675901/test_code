//
//  GRCServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRCServer.h"

GRCServer::GRCServer(deque<int>* rfd,deque<string>* rn){
//    roomAddr=ra;
    roomFD=rfd;
    roomName=rn;
    cout<<"Client Server service BEGIN"<<endl;
}
GRCServer::~GRCServer(){
    pthread_cancel(sendudp);
    pthread_cancel(recvudp);
    deque<int>::iterator iter;
    for (iter=roomFD->begin(); iter!=roomFD->end(); ++iter) {
        close(*iter);
    }
    delete udps;
    delete tcps;
    cout<<"Client Server service END"<<endl;
}
bool GRCServer::init(){
    udps=new UdpServer(50003,50002);
    if (udps->iniServer()) {
        pthread_create(&sendudp,NULL,GRCServer::findRoom,udps);
        pthread_create(&recvudp,NULL,GRCServer::recvRoom,this);
    }
    tcps=new TcpServer(50001);
    tcps->iniServer(-1);
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
    TcpServer* temptcps=tempgr->tcps;
    deque<int>* temproomFD=tempgr->roomFD;
    set<string>* ras=&(tempgr->roomAddr);
    while (true) {
        pthread_testcancel();
        int res;
        char tbuffer[9];
        if ((res=tempudps->recvMsg(tbuffer, 9))>0) {
            string temps=tbuffer;
            cout<<"net udp msg:"<<temps<<endl;
            string ss=GUtils::cptos(inet_ntoa(tempudps->getRemoteRecAddr()->sin_addr));
            cout<<"retome IP:"<<ss<<endl;
            if (ras->count(ss)==0) {
                cout<<"testtest1:"<<endl;
                ras->insert(ss);
                int roomi=temptcps->isConnect(ss.c_str(),50001);
                if (roomi>0) {
                    temproomFD->push_back(roomi);
                    GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
                }
            }
//            ras->insert(mapcom(ss,temps));
//            GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
        }
    }
    return NULL;
}