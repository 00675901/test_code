//
//  GRSServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRSServer.h"

GRSServer::GRSServer(int maxl,set<int>* cf,deque<string>* ml){
    maxLinsten=maxl;
    clientFD=cf;
    msglist=ml;
    FD_ZERO(&rfdset);
    FD_ZERO(&wfdset);
    FD_ZERO(&efdset);
    pthread_mutex_init(&mut, NULL);
    cout<<"Room Server service Begin"<<endl;
}
GRSServer::~GRSServer(){
    pthread_cancel(tidudp);
    pthread_cancel(tidtcp);
    FD_ZERO(&rfdset);
    FD_ZERO(&wfdset);
    FD_ZERO(&efdset);
    pthread_mutex_destroy(&mut);
    delete udps;
    delete tcps;
    cout<<"Room Server service End"<<endl;
}
bool GRSServer::init(){
    udps=new UdpServer(50002,50003);
    if (udps->iniServer()) {
        pthread_create(&tidudp,NULL,GRSServer::sendRoomService,udps);
    }
    tcps=new TcpServer(50001);
    if ((tcpsSocket=tcps->iniServer(maxLinsten))>0) {
        pthread_create(&tidtcp,NULL,GRSServer::listenRoomService,this);
    }
    cout<<"Room Server service Init:"<<maxLinsten<<endl;
    return true;
}

void* GRSServer::sendRoomService(void* obj){
    UdpServer *temp=(UdpServer *)obj;
    while (true) {
        char s[]="this is udp msg";
        temp->sendMsg(s, strlen(s));
        sleep(1);
    }
    return NULL;
}

void* GRSServer::listenRoomService(void* obj){
    GRSServer *temp=(GRSServer *)obj;
    TcpServer *tempTcps=temp->tcps;
    fd_set *tempRfdset=&(temp->rfdset);
    fd_set *tempWfdset=&(temp->wfdset);
    fd_set *tempEfdset=&(temp->efdset);
    set<int> *tempClientFD=temp->clientFD;
    deque<string> *tempMsglist=temp->msglist;
    int tempTcpsSocket=temp->tcpsSocket;
    int res;
    int maxFD=0;
    while (true) {
        FD_ZERO(tempRfdset);
        FD_ZERO(tempWfdset);
        FD_ZERO(tempEfdset);
        FD_SET(tempTcpsSocket, tempRfdset);
        FD_SET(tempTcpsSocket, tempEfdset);
        maxFD=maxFD>tempTcpsSocket?maxFD:tempTcpsSocket;
        set<int>::iterator iter;
        for (iter=tempClientFD->begin(); iter!=tempClientFD->end(); ++iter) {
            FD_SET(*iter, tempRfdset);
            FD_SET(*iter, tempWfdset);
            FD_SET(*iter, tempEfdset);
        }
        if (!tempClientFD->empty()) {
            --iter;
            maxFD=maxFD>*iter?maxFD:*iter;
        }
        int sel=select(maxFD+1, tempRfdset, tempWfdset, tempEfdset, NULL);
        if (sel<0) {
            if (EINTR==errno) {
                continue;
            }else{
                printf("select faild:%m");
            }
        }
        if (FD_ISSET(tempTcpsSocket, tempRfdset)) {
            if ((res=tempTcps->isAccept())>0) {
                tempClientFD->insert(res);
                string ts="a player join the room!";
                GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
                tempMsglist->push_back(ts);
                if (tempMsglist->size()>14) {
                    tempMsglist->pop_front();
                }
                GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
                for (iter=tempClientFD->begin(); iter!=tempClientFD->end(); ++iter) {
                    tempTcps->sendMsg(*iter,ts.c_str(),strlen(ts.c_str()));
                }
            }
        }
        iter=tempClientFD->begin();
        while (iter!=tempClientFD->end()) {
            if (FD_ISSET(*iter, tempRfdset)){
                char tt[8];
                int lenr=tempTcps->recvMsg(*iter,tt,8);
                if (lenr<=0) {
                    close(*iter);
                    string ts="a player leave the room!";
                    tempClientFD->erase(iter++);
                    temp->sendMsgToAll(ts.c_str());
                    tempMsglist->push_back(ts);
                    if (tempMsglist->size()>14) {
                        tempMsglist->pop_front();
                    }
                    GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
                    GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
                }else{
                    tempTcps->sendMsg(*iter,tt,8);
                    iter++;
                }
            }else{
                iter++;
            }
        }
    }
    return NULL;
}

void GRSServer::sendMsgToAll(const char* msg){
    int msgsize=strlen(msg);
//    cout<<"Msg length:"<<msgsize<<endl;
    set<int>::iterator iters=clientFD->begin();
    while (iters!=clientFD->end()) {
        tcps->sendMsg(*iters, msg, msgsize);
        iters++;
    }
}