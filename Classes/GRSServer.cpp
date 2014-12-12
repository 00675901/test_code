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
    pthread_mutex_init(&mut, NULL);
    cout<<"Room Server service BEGIN"<<endl;
}
GRSServer::~GRSServer(void){
    pthread_mutex_destroy(&mut);
    FD_ZERO(&rfdset);
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
    string tis="中文测试 ";
    tis.append(GUtils::itos(tls));
    while (true) {
        pthread_testcancel();
        int res;
        char tbuffer[8];
        sockaddr_in remoteRecAddr;
        if ((res=temp->recvMsg(tbuffer,8,&remoteRecAddr))>0) {
            const char* sa=inet_ntoa(remoteRecAddr.sin_addr);
            string temps=tbuffer;
            const char* s=tis.c_str();
            temp->sendMsg(sa,s,strlen(s));
        }
    }
    return NULL;
}

void GRSServer::startListenRoomService(int maxl,map<int,unsigned int>* cf,deque<string>* ml){
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
    map<int,unsigned int>::iterator iter;
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
    map<int,unsigned int> *tempClientFD=temp->clientFD;
    deque<string> *tempMsglist=temp->msglist;
    int tempTcpsSocket=temp->tcpsSocket;
    int res;
    int maxFD=0;
    typedef pair<int,unsigned int> tp;
    while (true) {
        pthread_testcancel();
        FD_ZERO(tempRfdset);
        FD_SET(tempTcpsSocket, tempRfdset);
        maxFD=maxFD>tempTcpsSocket?maxFD:tempTcpsSocket;
        map<int,unsigned int>::iterator iter;
        for (iter=tempClientFD->begin(); iter!=tempClientFD->end(); ++iter) {
            FD_SET(iter->first, tempRfdset);
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
            sockaddr_in remoteAddr;
            if ((res=tempTcps->isAccept(&remoteAddr))>0) {
                unsigned int reAddr=remoteAddr.sin_addr.s_addr;
//                tempClientFD->insert(tp(res,reAddr));
                map<int,unsigned int>::iterator iters=tempClientFD->begin();
                while (iters!=tempClientFD->end()) {
                    char ss[]="";
                    sprintf(ss,"%d",iter->second);
                    TcpServer::GCData tgcd;
                    tgcd.opcode=GCOPC_SIP;
                    tgcd.data=ss;
                    tempTcps->sendData(res, &tgcd);
                    iters++;
                }
                tempClientFD->insert(tp(res,reAddr));
                TcpServer::GCData tgcds;
                tgcds.opcode=GCOPC_SSNAME;
                tgcds.data="TEST_SERVER";
                tempTcps->sendData(res, &tgcds);
                
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
                TcpServer::GCData tgcd;
                int lenr=tempTcps->recvData(iter->first,&tgcd);
                if (lenr<=0) {
                    close(iter->first);
                    string ts="a player leave the room!";
                    tempClientFD->erase(iter++);
                    tempMsglist->push_back(ts);
                    if (tempMsglist->size()>14) {
                        tempMsglist->pop_front();
                    }
                    GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
                    GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
                }else{
                    iter++;
                }
            }else{
                iter++;
            }
        }
    }
    return NULL;
}

void GRSServer::sendMsgToAll(char* msg){
//    int msgsize=strlen(msg);
    map<int,unsigned int>::iterator iters=clientFD->begin();
    while (iters!=clientFD->end()) {
//        tcps->sendMsg(iters->first, msg);
        iters++;
    }
}