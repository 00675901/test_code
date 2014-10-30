//
//  GRCServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRCServer.h"

static GRCServer *gsnInstance;
GRCServer* GRCServer::shareInstance(){
    if (!gsnInstance) {
        gsnInstance=new GRCServer();
    }
    return gsnInstance;
}
GRCServer::GRCServer(void){
    FD_ZERO(&rfdset);
    pthread_mutex_init(&mut, NULL);
    cout<<"Client Server service BEGIN"<<endl;
}
GRCServer::~GRCServer(void){
    pthread_mutex_destroy(&mut);
    FD_ZERO(&rfdset);
    cout<<"Client Server service END"<<endl;
}

void GRCServer::startFindRoomService(map<int, string>* rli){
    roomlistInfo=rli;
    udps=new UdpServer(50003,50002,true);
    if (udps->iniServer()) {
        pthread_create(&sendudp,NULL,GRCServer::findRoom,udps);
        pthread_create(&recvudp,NULL,GRCServer::recvRoom,this);
        pthread_create(&listenRS,NULL,GRCServer::listenRoomStatus,this);
    }
}
void GRCServer::stopFindRoomService(){
    pthread_cancel(sendudp);
    pthread_cancel(recvudp);
    pthread_cancel(listenRS);
    roomStatus.clear();
    delete udps;
}
void* GRCServer::findRoom(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    UdpServer* tempudp=(UdpServer *)obj;
    while (true) {
        pthread_testcancel();
        char s[]="1";
        tempudp->sendMsg(s, 1);
        sleep(1);
    }
    return NULL;
}
void* GRCServer::recvRoom(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_testcancel();
    GRCServer* tempgr=(GRCServer*)obj;
    UdpServer* tempudps=tempgr->udps;
    int tls=tempudps->localSo;
    pthread_mutex_t* tempmut=&(tempgr->mut);
    fd_set *tempRfdset=&(tempgr->rfdset);
    map<int, int>* temproomlist=&(tempgr->roomStatus);
    map<int, string>* temproomlistInfo=tempgr->roomlistInfo;
    struct timeval ov;
    ov.tv_sec=3;
    ov.tv_usec=0;
    map<int, int>::iterator itm;
    typedef pair<int, int> tp;
    typedef pair<int, string> ts;
    while (true) {
        pthread_testcancel();
        FD_ZERO(tempRfdset);
        FD_SET(tls, tempRfdset);
        int sel=select(tls+1, tempRfdset, NULL, NULL, NULL);
        if (sel<0) {
            if (EINTR==errno) {
                continue;
            }else{
                printf("select faild:%m");
            }
        }
        if (FD_ISSET(tls, tempRfdset)){
            char tbuffer[16];
            int lenr=tempudps->recvMsg(tbuffer, 16);
            if (lenr>0) {
                string temprip=GUtils::cptos(inet_ntoa(tempudps->getRemoteRecAddr()->sin_addr));
                int rip=tempudps->getRemoteRecAddr()->sin_addr.s_addr;
                int testCount=6;
                pthread_mutex_lock(tempmut);
                itm=temproomlist->find(rip);
                if (itm==temproomlist->end()) {
                    temproomlist->insert(tp(rip,testCount));
                    temproomlistInfo->insert(ts(rip,tbuffer));
                    GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
                }else{
                    itm->second=testCount;
                }
                pthread_mutex_unlock(tempmut);
            }
        }
    }
    return NULL;
}
void* GRCServer::listenRoomStatus(void* obj){
    printf("listen Begin\n");
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GRCServer* temp=(GRCServer *)obj;
    pthread_mutex_t* tempmut=&(temp->mut);
    map<int, int>* temproomlist=&(temp->roomStatus);
    map<int, string>* temproomlistInfo=temp->roomlistInfo;
    map<int, int>::iterator itm;
    map<int, string>::iterator itminfo;
    while (true) {
        pthread_testcancel();
        pthread_mutex_lock(tempmut);
        itm=temproomlist->begin();
        while (itm!=temproomlist->end()) {
            (itm->second)--;
//            cout<<"room:"<<itm->first<<"-------"<<itm->second<<endl;
            if (0==itm->second) {
                cout<<"room:"<<itm->first<<" closed"<<endl;
                itminfo=temproomlistInfo->find(itm->first);
                temproomlistInfo->erase(itminfo);
                GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
                temproomlist->erase(itm++);
            }else{
                itm++;
            }
        }
        pthread_mutex_unlock(tempmut);
        sleep(1);
    }
    return NULL;
}

void GRCServer::startConnectService(){
    serverFD=-1;
    tcps=new TcpServer(50001);
    tcps->iniServer(-1);
}
void GRCServer::stopConnectService(){
    close(serverFD);
    delete tcps;
}
int GRCServer::connectRoom(int addr){
    serverFD=tcps->isConnect(addr, 50001);
    return serverFD;
}
