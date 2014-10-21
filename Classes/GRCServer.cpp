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
    pthread_cancel(listentcp);
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
    }
    tcps=new TcpServer(50001);
    if (tcps->iniServer(-1)) {
        pthread_create(&listentcp,NULL,GRCServer::listenRoomStatus,this);
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
        int re=temp->sendMsg(s, 1);
        printf("tttt:%d\n",re);
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
    int tttt=tempudps->localSo;
    TcpServer* temptcps=tempgr->tcps;
    pthread_mutex_t* tempmut=&(tempgr->mut);
    fd_set *tempRfdset=&(tempgr->rfdset);
    set<int>* temproomFD=tempgr->roomFD;
    set<string>* ras=&(tempgr->roomAddr);
    struct timeval ov;
    ov.tv_sec=3;
    ov.tv_usec=0;
    while (true) {
        pthread_testcancel();
        FD_ZERO(tempRfdset);
        FD_SET(tttt, tempRfdset);
        printf("select time---------------1\n");
        int sel=select(tttt+1, tempRfdset, NULL, NULL, &ov);
        printf("select time---------------2\n");
        if (sel<0) {
            if (EINTR==errno) {
                continue;
            }else{
                printf("select faild:%m");
            }
        }
        if (FD_ISSET(tttt, tempRfdset)){
            char tbuffer[9];
            int lenr=tempudps->recvMsg(tbuffer, 9);
            if (lenr>0) {
                printf("dadadadadadadadad\n");
            }
        }else{
            sleep(5);
            char s[]="9999";
            tempudps->sendMsg(s, 4);
            printf("clear data\n");
        }
    }
        
        
//        if ((res=tempudps->recvMsg(tbuffer, 9))>0) {
//            string temps=tbuffer;
//            cout<<"roomFD:"<<temproomFD->size()<<endl;
//            cout<<"net udp msg:"<<temps<<endl;
//            string ss=GUtils::cptos(inet_ntoa(tempudps->getRemoteRecAddr()->sin_addr));
//            cout<<"retome IP:"<<ss<<endl;
//            int roomi=temptcps->isConnect(ss.c_str(),50001);
//            printf("test------:%d",roomi);
//            if (roomi>0) {
//                pthread_mutex_lock(tempmut);
//                temproomFD->insert(roomi);
//                pthread_mutex_unlock(tempmut);
//                GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
//            }
//            ras->insert(mapcom(ss,temps));
//            GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
//        }
    return NULL;
}

void* GRCServer::listenRoomStatus(void* obj){
    printf("listen Begin\n");
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GRCServer* tempgr=(GRCServer*)obj;
    set<int>* tempfd=tempgr->roomFD;
    fd_set *tempRfdset=&(tempgr->rfdset);
    TcpServer *tempTcps=tempgr->tcps;
    pthread_mutex_t* tempmut=&(tempgr->mut);
    int maxFD=0;
    struct timeval ov;
    ov.tv_sec=1;
    ov.tv_usec=0;
    while (true) {
        pthread_testcancel();
        FD_ZERO(tempRfdset);
        set<int>::iterator iter;
        for (iter=tempfd->begin(); iter!=tempfd->end(); ++iter) {
            printf("setFD:%d",*iter);
            FD_SET(*iter, tempRfdset);
        }
        if (!tempfd->empty()) {
            --iter;
            maxFD=maxFD>*iter?maxFD:*iter;
        }
        int sel=select(maxFD+1, tempRfdset, NULL, NULL, &ov);
        if (sel<0) {
            if (EINTR==errno) {
                continue;
            }else{
                printf("select faild:%m");
            }
        }
        iter=tempfd->begin();
        while (iter!=tempfd->end()) {
            if (FD_ISSET(*iter, tempRfdset)){
                char tt[8];
                int lenr=tempTcps->recvMsg(*iter,tt,8);
                if (lenr<=0) {
                    pthread_mutex_lock(tempmut);
                    close(*iter);
                    tempfd->erase(iter++);
                    pthread_mutex_unlock(tempmut);
                    GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
                }else{
                    if (strcmp("0", tt)>0) {
                        printf("test_0");
                    }else if (strcmp("1", tt)>0){
                        printf("test_1");
                    }else{
                        printf("noMatch\n");
                    }
                    iter++;
                }
            }else{
                iter++;
            }
        }
    }
    return NULL;
}