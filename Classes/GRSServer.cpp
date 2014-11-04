//
//  GRSServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRSServer.h"

static GRSServer *gsnInstance;
GRSServer* GRSServer::shareInstance(){
    if (!gsnInstance) {
        gsnInstance=new GRSServer();
    }
    return gsnInstance;
}

GRSServer::GRSServer(void){
    FD_ZERO(&rfdset);
    FD_ZERO(&wfdset);
    FD_ZERO(&efdset);
    pthread_mutex_init(&mut, NULL);
    cout<<"Room Server service BEGIN"<<endl;
}
GRSServer::~GRSServer(void){
    pthread_mutex_destroy(&mut);
    FD_ZERO(&rfdset);
    FD_ZERO(&wfdset);
    FD_ZERO(&efdset);
    cout<<"Room Server service END"<<endl;
}

void GRSServer::startSendRoomService(){
    udps=new UdpServer(52156,52157,false);
    if (udps->iniServer()) {
        pthread_create(&tidudp,NULL,GRSServer::sendRoomService,this);
    }
}
void GRSServer::stopSendRoomService(){
    pthread_cancel(tidudp);
    delete udps;
}
void* GRSServer::sendRoomService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GRSServer *tempgr=(GRSServer *)obj;
    UdpServer *temp=tempgr->udps;
    int tls=temp->localSo;
    string tis="Game Room ";;
    tis.append(GUtils::itos(tls));
    while (true) {
        pthread_testcancel();
        int res;
        char tbuffer[8];
        if ((res=temp->recvMsg(tbuffer, 8))>0) {
            const char* sa=inet_ntoa(temp->getRemoteRecAddr()->sin_addr);
            string temps=tbuffer;
            const char* s=tis.c_str();
            temp->sendMsg(sa,s,strlen(s));
        }
    }
    return NULL;
}

void GRSServer::startListenRoomService(int maxl,map<int,int>* cf,deque<string>* ml){
    maxLinsten=maxl;
    clientFD=cf;
    msglist=ml;
    tcps=new TcpServer(52125);
    if ((tcpsSocket=tcps->iniServer(maxLinsten))>0) {
        pthread_create(&tidtcp,NULL,GRSServer::listenRoomService,this);
    }
}
void GRSServer::stopListenRoomService(){
    pthread_cancel(tidtcp);
    map<int,int>::iterator iter;
    for (iter=clientFD->begin(); iter!=clientFD->end(); ++iter) {
        close(iter->first);
    }
    clientFD->clear();
    msglist->clear();
    delete tcps;
}
void* GRSServer::listenRoomService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GRSServer *temp=(GRSServer *)obj;
    TcpServer *tempTcps=temp->tcps;
    fd_set *tempRfdset=&(temp->rfdset);
    fd_set *tempWfdset=&(temp->wfdset);
    fd_set *tempEfdset=&(temp->efdset);
    map<int,int> *tempClientFD=temp->clientFD;
    deque<string> *tempMsglist=temp->msglist;
    int tempTcpsSocket=temp->tcpsSocket;
    int res;
    int maxFD=0;
    typedef pair<int, int> tp;
    while (true) {
        pthread_testcancel();
        FD_ZERO(tempRfdset);
        FD_ZERO(tempWfdset);
        FD_ZERO(tempEfdset);
        FD_SET(tempTcpsSocket, tempRfdset);
        FD_SET(tempTcpsSocket, tempEfdset);
        maxFD=maxFD>tempTcpsSocket?maxFD:tempTcpsSocket;
        map<int,int>::iterator iter;
        for (iter=tempClientFD->begin(); iter!=tempClientFD->end(); ++iter) {
            FD_SET(iter->first, tempRfdset);
            FD_SET(iter->first, tempWfdset);
            FD_SET(iter->first, tempEfdset);
        }
        if (!tempClientFD->empty()) {
            --iter;
            maxFD=maxFD>(iter->first)?maxFD:(iter->first);
        }
        int sel=select(maxFD+1, tempRfdset, NULL, NULL, NULL);
        if (sel<0) {
            if (EINTR==errno) {
                continue;
            }else{
                printf("select faild:%m");
            }
        }
        if (FD_ISSET(tempTcpsSocket, tempRfdset)) {
            if ((res=tempTcps->isAccept())>0) {
                int reAddr=tempTcps->getRemoteRecAddr()->sin_addr.s_addr;
                map<int,int>::iterator iters=tempClientFD->begin();
                while (iters!=tempClientFD->end()) {
                    const char* ss=GUtils::itoc(iter->second);
                    int msgsize=strlen(ss);
                    int test=tempTcps->sendMsg(res, ss, msgsize);
                    printf("server send:%d\n",test);
                    iters++;
                }
                tempClientFD->insert(tp(res,reAddr));
                GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
                
                string ts="a player join the room!";
                tempMsglist->push_back(ts);
                if (tempMsglist->size()>14) {
                    tempMsglist->pop_front();
                }
                GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
            }
        }
        iter=tempClientFD->begin();
        while (iter!=tempClientFD->end()) {
            if (FD_ISSET(iter->first, tempRfdset)){
                char tt[8];
                int lenr=tempTcps->recvMsg(iter->first,tt,8);
                if (lenr<=0) {
                    close(iter->first);
                    string ts="a player leave the room!";
                    tempClientFD->erase(iter++);
//                    temp->sendMsgToAll(ts.c_str());
                    tempMsglist->push_back(ts);
                    if (tempMsglist->size()>14) {
                        tempMsglist->pop_front();
                    }
                    GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
                    GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
                }else{
//                    tempTcps->sendMsg(iter->first,tt,8);
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
    map<int,int>::iterator iters=clientFD->begin();
    while (iters!=clientFD->end()) {
        tcps->sendMsg(iters->first, msg, msgsize);
        iters++;
    }
}