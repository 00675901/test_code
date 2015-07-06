//
//  GRSServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GNSServer.h"

static GNSServer *gsnInstance;
GNSServer* GNSServer::shareInstance(){
    if (!gsnInstance) {
        gsnInstance=new GNSServer();
    }
    return gsnInstance;
}

GNSServer::GNSServer(void){
    serverStatus=SERVER_STOP;
    cout<<"Server service BEGIN"<<endl;
}
GNSServer::~GNSServer(void){
    cout<<"Server service END"<<endl;
}

//Send Room Service function
void GNSServer::startResponseService(int maxl,const char* uname){
    if (SERVER_STOP==serverStatus) {
        serverStatus=SERVER_S_RUN;
        udps=new UdpServer(52156,52157,false);
        if (udps->iniServer()) {
            pthread_create(&tidResponseService,NULL,GNSServer::responseService,this);
            maxLinsten=maxl;
            localName=uname;
            tcps=new TcpServer(52125);
            if ((localTcpFD=tcps->iniServer(maxLinsten))>0) {
                pthread_create(&tidListenRoomService,NULL,GNSServer::listenNetService,this);
            }
        }
    }
}
void GNSServer::pauseResponseService(){
    if (SERVER_S_RUN==serverStatus) {
        pthread_cancel(tidResponseService);
        serverStatus=SERVER_S_PAUSE;
    }
}
void GNSServer::resumeResponseService(){
    if (SERVER_S_PAUSE==serverStatus) {
        pthread_create(&tidResponseService,NULL,GNSServer::responseService,this);
        serverStatus=SERVER_S_RUN;
    }
}
void GNSServer::stopResponseService(){
    if(SERVER_S_RUN==serverStatus){
        pthread_cancel(tidResponseService);
        pthread_cancel(tidListenRoomService);
        map<int,unsigned int>::iterator iter;
        for (iter=remoteFDIP.begin(); iter!=remoteFDIP.end(); ++iter) {
            close(iter->first);
        }
        remoteFDIP.clear();
        delete tcps;
        delete udps;
        serverStatus=SERVER_STOP;
    }
}
void* GNSServer::responseService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    GNSServer *tempgr=(GNSServer *)obj;
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

//void GNSServer::sendMsgToAll(char* msg){
////    int msgsize=strlen(msg);
//    map<int,unsigned int>::iterator iters=remoteFDIP.begin();
//    while (iters!=remoteFDIP.end()) {
////        tcps->sendMsg(iters->first, msg);
//        iters++;
//    }
//}