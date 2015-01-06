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
    pthread_mutex_init(&mut, NULL);
    FD_ZERO(&rfdset);
    FD_ZERO(&trfdset);
    cout<<"Client Server service BEGIN"<<endl;
}
GRCServer::~GRCServer(void){
    pthread_mutex_destroy(&mut);
    FD_ZERO(&rfdset);
    FD_ZERO(&trfdset);
    cout<<"Client Server service END"<<endl;
}

void GRCServer::startFindRoomService(map<int, string>* rli){
    roomlistInfo=rli;
    udps=new UdpServer(52157,52156,true);
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
        tempudp->sendMsg(s);
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
            char tbuffer[1024];
            sockaddr_in remoteRecAddr;
            int lenr=tempudps->recvMsg(tbuffer,1024,&remoteRecAddr);
            if (lenr>0) {
                string temprip=GUtils::cptos(inet_ntoa(remoteRecAddr.sin_addr));
                int rip=remoteRecAddr.sin_addr.s_addr;
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
    pthread_testcancel();
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

bool GRCServer::initConnectService(int addr){
    tcps=new TcpServer(52125);
    if ((localFD=tcps->iniServer(10))>0) {
        serverFD=tcps->isConnect(addr, 52125);
        if (serverFD>0) {
            typedef pair<int, int> tp;
            romateFDIP.insert(tp(serverFD,addr));
            return true;
        }
    }
    return false;
}

void GRCServer::startConnectService(const char* uname){
    localName=uname;
    
    TcpServer::GCData tgcd;
    tgcd.opcode=GCOPC_SCNAME;
    tgcd.data=localName;
    tcps->sendData(serverFD,&tgcd);
    
    pthread_create(&listenRoc,NULL,GRCServer::listenRoomService,this);
}

void GRCServer::stopConnectService(){
    pthread_cancel(listenRoc);
    map<int,unsigned int>::iterator iter;
    for (iter=romateFDIP.begin(); iter!=romateFDIP.end(); ++iter) {
        close(iter->first);
    }
    romateFDIP.clear();
    romateFDName.clear();
    loglist.clear();
    close(localFD);
    delete tcps;
}
void* GRCServer::listenRoomService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_testcancel();
    GRCServer *temp=(GRCServer *)obj;
    TcpServer *tempTcps=temp->tcps;
    int tempLocalFD=temp->localFD;
    fd_set *temptRfdset=&(temp->trfdset);
    map<int,unsigned int> *tempRemotaFD=&(temp->romateFDIP);
    map<int,string> *tempRemotaName=&(temp->romateFDName);
    deque<string> *tempMsglist=&(temp->loglist);
    string tempLocalName=temp->localName;
    int res;
    int maxFD=0;
//    struct timeval ov;
//    ov.tv_sec=1;
//    ov.tv_usec=0;
    typedef pair<int,unsigned int> tp;
    typedef pair<int,string> ta;
    string ts1="a player join the room!";
    string ts2="a player leave the room!";
    while (true) {
        pthread_testcancel();
        FD_ZERO(temptRfdset);
        FD_SET(tempLocalFD, temptRfdset);
        maxFD=maxFD>tempLocalFD?maxFD:tempLocalFD;
        map<int,unsigned int>::iterator iter;
        for (iter=tempRemotaFD->begin(); iter!=tempRemotaFD->end(); ++iter) {
            FD_SET(iter->first, temptRfdset);
        }
        if (!tempRemotaFD->empty()) {
            --iter;
            maxFD=maxFD>(iter->first)?maxFD:(iter->first);
        }
        int sel=select(maxFD+1, temptRfdset, NULL, NULL, NULL);
        if (sel<0) {
            if (EINTR==errno) {
                continue;
            }else{
                printf("select faild:%m");
            }
        }
        if (FD_ISSET(tempLocalFD, temptRfdset)) {
            sockaddr_in remoteAddr;
            if ((res=tempTcps->isAccept(&remoteAddr))>0) {
                unsigned int reAddr=remoteAddr.sin_addr.s_addr;
                TcpServer::GCData tgcd;
                tgcd.opcode=GCOPC_SCNAME;
                tgcd.data=tempLocalName;
                tempTcps->sendData(res,&tgcd);
                tempRemotaFD->insert(tp(res,reAddr));
            }else{
                printf("客户端连接客户端失败~");
            }
        }
        iter=tempRemotaFD->begin();
        while (iter!=tempRemotaFD->end()) {
            if (FD_ISSET(iter->first, temptRfdset)){
                TcpServer::GCData tgcd;
                int lenr=tempTcps->recvData(iter->first,&tgcd);
                if (lenr<=0) {
                    close(iter->first);
                    
                    map<int,string>::iterator tempiter=tempRemotaName->find(iter->first);
                    if (tempiter!=tempRemotaName->end()) {
                        tempRemotaName->erase(tempiter);
                    }
                    tempRemotaFD->erase(iter++);
                    tempMsglist->push_back(ts2);
                    if (tempMsglist->size()>14) {
                        tempMsglist->pop_front();
                    }
                }else{
                    string topc=tgcd.opcode;
                    string tdat=tgcd.data;
                    if (topc.compare(GCOPC_SIP)==0) {
                        int reip=GUtils::ctoi(tdat.c_str());
                        printf("remota Msg:%s(--%d--)\n",tdat.c_str(),reip);
                        int tempremo=tempTcps->isConnect(reip, 52125);
                        if (tempremo>0) {
                            TcpServer::GCData temptd;
                            temptd.opcode=GCOPC_SCNAME;
                            temptd.data=tempLocalName;
                            tempTcps->sendData(tempremo,&temptd);
                            tempRemotaFD->insert(tp(tempremo,reip));
                        }
                        tempMsglist->push_back(ts1);
                        if (tempMsglist->size()>14) {
                            tempMsglist->pop_front();
                        }
                    }else if (topc.compare(GCOPC_SSNAME)==0||topc.compare(GCOPC_SCNAME)==0){
                        tempRemotaName->insert(ta(iter->first,tdat));
                        string ts="a player join the room!";
                        tempMsglist->push_back(ts);
                        if (tempMsglist->size()>14) {
                            tempMsglist->pop_front();
                        }
                    }
                    iter++;
                }
                GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
                GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
            }else{
                iter++;
            }
        }
    }
    return NULL;
}




