//
//  GRCServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GNCServer.h"

static GNCServer *gsnInstance;
GNCServer* GNCServer::shareInstance(){
    if (!gsnInstance) {
        gsnInstance=new GNCServer();
    }
    return gsnInstance;
}
GNCServer::GNCServer(void){
    serverStatus=SERVER_STOP;
    pthread_mutex_init(&gncMut, NULL);
    cout<<"Client service BEGIN"<<endl;
}
GNCServer::~GNCServer(void){
    pthread_mutex_destroy(&gncMut);
    cout<<"Client service END"<<endl;
}

void GNCServer::startSearchService(){
    if (SERVER_STOP==serverStatus) {
        serverStatus=SERVER_C_RUN;
        udps=new UdpServer(52157,52156,true);
        if (udps->iniServer()) {
            pthread_create(&tidSearchServer,NULL,GNCServer::searchServer,udps);
            pthread_create(&tidRecvServer,NULL,GNCServer::recvServerList,this);
        }
    }
}
void GNCServer::pauseSearchService(){
    if (SERVER_C_RUN==serverStatus) {
        serverStatus=SERVER_C_PAUSE;
        pthread_cancel(tidSearchServer);
        pthread_cancel(tidRecvServer);
    }
}
void GNCServer::resumeSearchService(){
    if (SERVER_C_PAUSE==serverStatus) {
        serverStatus=SERVER_C_RUN;
        pthread_create(&tidSearchServer,NULL,GNCServer::searchServer,this);
        pthread_create(&tidRecvServer,NULL,GNCServer::recvServerList,this);
    }
}
void GNCServer::stopSearchService(){
    if (SERVER_C_RUN==serverStatus) {
        pthread_cancel(tidSearchServer);
        pthread_cancel(tidRecvServer);
        delete udps;
        serverStatus=SERVER_STOP;
    }
}
void* GNCServer::searchServer(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GNCServer *temp=(GNCServer *)obj;
    UdpServer* tempudp=temp->udps;
    pthread_mutex_t* tempmut=&(temp->gncMut);
    map<int, int>* temproomlist=&(temp->serverListStatus);
    map<int, int>::iterator itm;
    map<int, string>* temproomlistInfo=&(temp->serverList);
    map<int, string>::iterator itminfo;
    while (true) {
        pthread_testcancel();
        char s[]="1";
        tempudp->sendMsg(s);
        pthread_mutex_lock(tempmut);
        itm=temproomlist->begin();
        while (itm!=temproomlist->end()) {
            (itm->second)--;
            if (0==itm->second) {
                cout<<"room:"<<itm->first<<" closed"<<endl;
                itminfo=temproomlistInfo->find(itm->first);
                temproomlistInfo->erase(itminfo);
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

void* GNCServer::recvServerList(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_testcancel();
    GNCServer* tempgr=(GNCServer*)obj;
    UdpServer* tempudps=tempgr->udps;
    pthread_mutex_t* tempmut=&(tempgr->gncMut);
    map<int, int>* temproomlist=&(tempgr->serverListStatus);
    map<int, string>* temproomlistInfo=&(tempgr->serverList);
    map<int, int>::iterator itm;
    typedef pair<int, int> tp;
    typedef pair<int, string> ts;
    while (true) {
        pthread_testcancel();
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
    return NULL;
}

void GNCServer::startConnectService(int addr){
    tcps=new TcpServer(52125);
    if ((localTcpFD=tcps->iniServer(10))>0) {
        int remoteFD=tcps->isConnect(addr, 52125);
        if (remoteFD>0) {
            typedef pair<int, int> tp;
            remoteFDIP.insert(tp(remoteFD,addr));
            pthread_create(&tidListenNetService,NULL,GNCServer::listenNetService,this);
            TcpServer::GCData tgcd;
            tgcd.opcode=GNOC_GIP;
            tgcd.data="0";
            tcps->sendData(remoteFD,&tgcd);
        }
    }
}

void GNCServer::stopConnectService(){
    pthread_cancel(tidListenNetService);
    map<int,unsigned int>::iterator iter;
    for (iter=remoteFDIP.begin(); iter!=remoteFDIP.end(); ++iter) {
        close(iter->first);
    }
    remoteFDIP.clear();
    close(localTcpFD);
    delete tcps;
}

