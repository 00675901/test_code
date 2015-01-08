//
//  GNetServer.cpp
//  TestCocos2dx
//
//  Created by othink on 15/1/6.
//
//

#include "GNetServer.h"

//system function
static GNetServer *gsnInstance;
GNetServer* GNetServer::shareInstance(){
    if (!gsnInstance) {
        gsnInstance=new GNetServer();
    }
    return gsnInstance;
}
GNetServer::GNetServer(void){
    serverStatus=SERVER_STOP;
    pthread_mutex_init(&mut, NULL);
    printf("GNetServer BEGIN\n");
}
GNetServer::~GNetServer(void){
    pthread_mutex_destroy(&mut);
    printf("GNetServer END\n");
}

//use member function
unsigned int GNetServer::getLocalIP(){
    return localIP;
}
const char* GNetServer::getLocalName(){
    return localName;
}
map<int,unsigned int> GNetServer::getRemoteFDIP(){
    return remoteFDIP;
}
map<int,string> GNetServer::getRemoteFDName(){
    return remoteFDName;
}
deque<string> GNetServer::getLoglist(){
    return loglist;
}

//Send Room Service function
void GNetServer::startResponseService(int maxl,const char* uname){
    if (SERVER_STOP==serverStatus) {
        serverStatus=SERVER_S_RUN;
        udps=new UdpServer(52156,52157,false);
        if (udps->iniServer()) {
            pthread_create(&tidRoomService,NULL,GNetServer::responseService,this);
            maxLinsten=maxl;
            localName=uname;
            tcps=new TcpServer(52125);
            if ((localTcpFD=tcps->iniServer(maxLinsten))>0) {
                pthread_create(&tidListenRoomService,NULL,GNetServer::listenNetService,this);
            }
        }
    }
}
void GNetServer::pauseResponseService(){
    if (SERVER_S_RUN==serverStatus) {
        pthread_cancel(tidRoomService);
        serverStatus=SERVER_S_PAUSE;
    }
}
void GNetServer::resumeResponseService(){
    if (SERVER_S_PAUSE==serverStatus) {
        pthread_create(&tidRoomService,NULL,GNetServer::responseService,this);
        serverStatus=SERVER_S_RUN;
    }
}
void GNetServer::stopResponseService(){
    if(SERVER_S_RUN==serverStatus){
        pthread_cancel(tidRoomService);
        pthread_cancel(tidListenRoomService);
        map<int,unsigned int>::iterator iter;
        for (iter=remoteFDIP.begin(); iter!=remoteFDIP.end(); ++iter) {
            close(iter->first);
        }
        remoteFDIP.clear();
        remoteFDName.clear();
        loglist.clear();
        delete tcps;
        delete udps;
        serverStatus=SERVER_STOP;
    }
}
void* GNetServer::responseService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GNetServer *tempgr=(GNetServer *)obj;
    UdpServer *temp=tempgr->udps;
    string tis=tempgr->localName;
    tis.append("的房间");
    while (true) {
        pthread_testcancel();
        int res;
        char tbuffer[8];
        sockaddr_in remoteRecAddr;
        if ((res=temp->recvMsg(tbuffer,8,&remoteRecAddr))>0) {
            const char* sa=inet_ntoa(remoteRecAddr.sin_addr);
            string temps=tbuffer;
            const char* s=tis.c_str();
            temp->sendMsg(sa,s);
        }
    }
    return NULL;
}

void* GNetServer::listenNetService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_testcancel();
    GNetServer *temp=(GNetServer *)obj;
    TcpServer *tempTcps=temp->tcps;
    int tempLocalFD=temp->localFD;
    fd_set *temptRfdset=&(temp->rfdset);
    map<int,unsigned int> *tempRemotaFD=&(temp->remoteFDIP);
    map<int,string> *tempRemotaName=&(temp->remoteFDName);
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
                tempRemotaFD->insert(tp(res,reAddr));
            }
        }
        iter=tempRemotaFD->begin();
        while (iter!=tempRemotaFD->end()) {
            if (FD_ISSET(iter->first, temptRfdset)){
                TcpServer::GCData tgcd;
                int lenr=tempTcps->recvData(iter->first,&tgcd);
                if (lenr<=0) {
                    close(iter->first);
                    tempRemotaFD->erase(iter++);
                }else{
                    string topc=tgcd.opcode;
                    string tdat=tgcd.data;
                    if (topc.compare(GNOC_SIP)==0) {
                        int reip=GUtils::ctoi(tdat.c_str());
                        printf("remota Msg:%s(--%d--)\n",tdat.c_str(),reip);
                        int tempremo=tempTcps->isConnect(reip, 52125);
                        if (tempremo>0) {
                            tempRemotaFD->insert(tp(tempremo,reip));
                        }
                    }else if (topc.compare(GNOC_GIP)==0){
                        map<int,unsigned int>::iterator iters;
                        for (iters=tempRemotaFD->begin(); iters!=tempRemotaFD->end(); ++iters) {
                            tgcd.opcode=GNOC_SIP;
                            tgcd.data=iter->second;
                            tempTcps->sendData(iter->first,&tgcd);
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

void GNetServer::startSearchService(){
    if (SERVER_STOP==serverStatus) {
        serverStatus=SERVER_C_RUN;
        udps=new UdpServer(52157,52156,true);
        if (udps->iniServer()) {
            pthread_create(&tidSearchServer,NULL,GNetServer::searchServer,udps);
            pthread_create(&tidRecvServer,NULL,GNetServer::recvServerList,this);
        }
    }
}
void GNetServer::pauseSearchService(){
    if (SERVER_C_RUN==serverStatus) {
        serverStatus=SERVER_C_PAUSE;
        pthread_cancel(tidSearchServer);
        pthread_cancel(tidRecvServer);
    }
}
void GNetServer::resumeSearchService(){
    if (SERVER_C_PAUSE==serverStatus) {
        serverStatus=SERVER_C_RUN;
        pthread_create(&tidSearchServer,NULL,GNetServer::searchServer,this);
        pthread_create(&tidRecvServer,NULL,GNetServer::recvServerList,this);
    }
}
void GNetServer::stopSearchService(){
    if (SERVER_C_RUN==serverStatus) {
        pthread_cancel(tidSearchServer);
        pthread_cancel(tidRecvServer);
        delete udps;
        serverStatus=SERVER_STOP;
    }
}
void* GNetServer::searchServer(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GNetServer *temp=(GNetServer *)obj;
    UdpServer* tempudp=temp->udps;
    pthread_mutex_t* tempmut=&(temp->mut);
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
void* GNetServer::recvServerList(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_testcancel();
    GNetServer* tempgr=(GNetServer*)obj;
    UdpServer* tempudps=tempgr->udps;
    pthread_mutex_t* tempmut=&(tempgr->mut);
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
//                GSNotificationPool::shareInstance()->postNotification("updateRoomList", NULL);
            }else{
                itm->second=testCount;
            }
            pthread_mutex_unlock(tempmut);
        }
    }
    return NULL;
}

void GNetServer::startConnectService(int addr){
    if (SERVER_STOP==serverStatus){
        serverStatus=SERVER_C_TCP_RUN;
        tcps=new TcpServer(52125);
        if ((localFD=tcps->iniServer(10))>0) {
            int remoteFD=tcps->isConnect(addr, 52125);
            if (remoteFD>0) {
                pthread_create(&tidConnectService,NULL,GNetServer::listenNetService,this);
                typedef pair<int, int> tp;
                remoteFDIP.insert(tp(remoteFD,addr));
                TcpServer::GCData tgcd;
                tgcd.opcode=GNOC_GIP;
                tgcd.data="0";
                tcps->sendData(remoteFD,&tgcd);
            }
        }
    }
}

void GNetServer::stopConnectService(){
    if (SERVER_C_TCP_RUN==serverStatus){
        pthread_cancel(tidConnectService);
        map<int,unsigned int>::iterator iter;
        for (iter=remoteFDIP.begin(); iter!=remoteFDIP.end(); ++iter) {
            close(iter->first);
        }
        remoteFDIP.clear();
        remoteFDName.clear();
        loglist.clear();
        close(localFD);
        delete tcps;
        serverStatus=SERVER_STOP;
    }
}
