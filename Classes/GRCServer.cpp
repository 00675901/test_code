//
//  GRCServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRCServer.h"

GRCServer::GRCServer(set<int>* rfd,deque<string>* rn){
//    roomAddr=ra;
    roomFD=rfd;
    roomName=rn;
    FD_ZERO(&rfdset);
    pthread_mutex_init(&mut, NULL);
    cout<<"Client Server service BEGIN"<<endl;
}
GRCServer::~GRCServer(){
    pthread_cancel(sendudp);
    pthread_cancel(recvudp);
    pthread_cancel(listenRS);
    pthread_mutex_destroy(&mut);
    FD_ZERO(&rfdset);
    set<int>::iterator iter;
    for (iter=roomFD->begin(); iter!=roomFD->end(); ++iter) {
        close(*iter);
    }
    delete udps;
    delete tcps;
    cout<<"Client Server service END"<<endl;
}
bool GRCServer::init(){
    udps=new UdpServer(50003,50002,true);
    if (udps->iniServer()) {
        pthread_create(&sendudp,NULL,GRCServer::findRoom,udps);
        pthread_create(&recvudp,NULL,GRCServer::recvRoom,this);
        pthread_create(&listenRS,NULL,GRCServer::listenRoomStatus,this);
    }
    
    tcps=new TcpServer(50001);
    if (tcps->iniServer(-1)) {
        
    }
    return true;
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
    map<string, int>* temproomlist=&(tempgr->roomlistInfo);
    struct timeval ov;
    ov.tv_sec=3;
    ov.tv_usec=0;
    map<string, int>::iterator itm;
    typedef pair<string, int> tp;
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
            char tbuffer[9];
            int lenr=tempudps->recvMsg(tbuffer, 9);
            if (lenr>0) {
                string rip=GUtils::cptos(inet_ntoa(tempudps->getRemoteRecAddr()->sin_addr));
                int testCount=6;
                pthread_mutex_lock(tempmut);
                itm=temproomlist->find(rip);
                if (itm==temproomlist->end()) {
                    temproomlist->insert(tp(rip,testCount));
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
    map<string, int>* temproomlist=&(temp->roomlistInfo);
    map<string, int>::iterator itm;
    while (true) {
        pthread_testcancel();
        pthread_mutex_lock(tempmut);
        itm=temproomlist->begin();
        while (itm!=temproomlist->end()) {
            (itm->second)--;
            cout<<"room:"<<itm->first<<"-------"<<itm->second<<endl;
            if (0==itm->second) {
                cout<<"room:"<<itm->first<<" closed"<<endl;
                temproomlist->erase(itm++);
                GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
            }else{
                itm++;
            }
        }
        pthread_mutex_unlock(tempmut);
        sleep(1);
    }
    return NULL;
}