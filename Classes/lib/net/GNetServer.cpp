//
//  GNetServer.cpp
//  Cocos2dx
//

#include "GNetServer.h"
using namespace std;
static GNetServer* GNetServerInstance;
GNetServer* GNetServer::shareInstance(){
    if (!GNetServerInstance) {
        GNetServerInstance=new GNetServer();
    }
    return GNetServerInstance;
}

//use member function
unsigned int* GNetServer::getLocalIP(){
    return &localIP;
}
const char* GNetServer::getLocalName(){
    return localName;
}
std::map<int,unsigned int>* GNetServer::getRemoteFDIP(){
    return &remoteFDIP;
}
std::map<unsigned int, std::string>* GNetServer::getTempUdpMap(){
    return &udpMap;
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
                pthread_create(&tidListenSConnectService,NULL,GNetServer::listenSNetService,this);
//                pthread_create(&tidListenSConnectService,NULL,GNetServer::listenSConnectServer,this);
//                pthread_create(&tidListenSRemotaService,NULL,GNetServer::listenSRemotaServer,this);
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
        pthread_cancel(tidListenSConnectService);
//        pthread_cancel(tidListenSRemotaService);
        std::map<int,unsigned int>::iterator iter;
        for (iter=remoteFDIP.begin(); iter!=remoteFDIP.end(); ++iter) {
            close(iter->first);
        }
        remoteFDIP.clear();
        close(localTcpFD);
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
    std::string tis=tempgr->localName;
    tis.append("的房间");
    while (true) {
        pthread_testcancel();
        long res=0;
        char tbuffer[8];
        sockaddr_in remoteRecAddr;
        res=temp->recvMsg(tbuffer,8,&remoteRecAddr);
        if (res>0) {
            const char* sa=inet_ntoa(remoteRecAddr.sin_addr);
            std::string temps=tbuffer;
            const char* s=tis.c_str();
            temp->sendMsg(sa,s);
        }
    }
    return NULL;
}

void* GNetServer::listenSNetService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_testcancel();
    GNetServer *temp=(GNetServer *)obj;
    TcpServer *tempTcps=temp->tcps;
    int tempLocalFD=temp->localTcpFD;
    fd_set *temptRfdset=&(temp->rfdset);
    std::map<int,unsigned int> *tempRemotaFD=&(temp->remoteFDIP);
    int maxFD=0;
    struct timeval tv;
    while (true) {
        pthread_testcancel();
        FD_ZERO(temptRfdset);
        FD_SET(tempLocalFD, temptRfdset);
        maxFD=tempLocalFD;
        std::map<int,unsigned int>::iterator iter;
        for (iter=tempRemotaFD->begin(); iter!=tempRemotaFD->end(); ++iter) {
            FD_SET(iter->first, temptRfdset);
            maxFD=maxFD>(iter->first)?maxFD:(iter->first);
        }
        tv.tv_sec=1;
        tv.tv_usec=0;
        int sel=select(maxFD+1, temptRfdset, NULL, NULL, NULL);
        printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        
        if (sel<0) {
            if (EINTR==errno) {
                continue;
            }else{
                printf("select faild:%m");
            }
        }
        printf("++++++++++++++++++++++++++++++++++++++\n");
        if (FD_ISSET(tempLocalFD, temptRfdset)) {
            sockaddr_in remoteAddr;
            int tempRFD;
            printf("----------------------\n");
            //            unsigned int remoteaddrlen=sizeof(remoteAddr);
            //            accept(tempLocalFD, (sockaddr *)&remoteAddr, &remoteaddrlen)
            if ((tempRFD=tempTcps->isAccept(&remoteAddr))>0) {
                printf("--------------------------------------------------\n");
                unsigned int reAddr=remoteAddr.sin_addr.s_addr;
                for (iter=tempRemotaFD->begin();iter!=tempRemotaFD->end(); ++iter) {
                    std::cout<<"ip:"<<iter->second<<std::endl;
                    //server send all ip
                    GNPacket tgcd;
                    tgcd.sysCode=SEND_IP;
                    tgcd.data=GUtils::itos(iter->second);
                    tempTcps->sendData(tempRFD,&tgcd);
                }
                tempRemotaFD->insert(std::make_pair(tempRFD,reAddr));
                //server send name;
                std::string tempn(temp->getLocalName());
                GNPacket msg;
                msg.sysCode=PLAYER_NAME;
                msg.data=tempn;
                
                printf("Server SendSend Content Packet:%d,%d,%s,%d,%s\n",msg.sysCode,msg.origin,msg.UUID.c_str(),msg.NPCode,msg.data.c_str());
                
                tempTcps->sendData(tempRFD,&msg);
            }
        }
        iter=tempRemotaFD->begin();
        while (iter!=tempRemotaFD->end()) {
            if (FD_ISSET(iter->first, temptRfdset)){
                GNPacket msg;
                long lenr=tempTcps->recvData(iter->first,&msg);
                printf("Server Recv Content Packet:%d,%d,%s,%d,%s\n",msg.sysCode,msg.origin,msg.UUID.c_str(),msg.NPCode,msg.data.c_str());
                if (lenr<=0) {
                    close(iter->first);
                    tempRemotaFD->erase(iter++);
                    //系统通知有连接断开
                    GNPacket smsg;
                    smsg.sysCode=DISCONNECTION;
                    smsg.origin=msg.origin;
                    temp->notificationSystemData(smsg);
                }else{
                    int tcd=msg.sysCode;
                    if (REPLAYER_NAME==tcd) {
                        //系统通知有新连接
                        GNPacket smsg;
                        smsg.sysCode=NEWCONNECTION;
                        smsg.origin=msg.origin;
                        smsg.data=msg.data;
                        temp->notificationSystemData(smsg);
                    }else{
                        temp->distributeData(msg.UUID,msg);
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

//void* GNetServer::listenSConnectServer(void* obj){
//    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
//    pthread_testcancel();
//    GNetServer *th_obj=(GNetServer *)obj;
//    TcpServer *thTcps=th_obj->tcps;
//    pthread_mutex_t* thMut=&(th_obj->remoteFDIPMutex);
//    pthread_mutex_t* thNetMut=&(th_obj->netMutex);
//    //    int tempLocalFD=temp->localFD;
//    std::map<int,unsigned int> *thRemotaFD=&(th_obj->remoteFDIP);
//    std::map<int,int> *thFdStatus=&(th_obj->fdStatusMap);
//    std::map<int,unsigned int>::iterator iter;
//    std::map<int,unsigned int>::iterator enditer;
//    while (true) {
//        printf("gadgadsfgsdfgsdfgsdfg\n");
//        sockaddr_in remoteAddr;
//        int tempRFD;
//        if ((tempRFD=thTcps->isAccept(&remoteAddr))>0) {
//            unsigned int reAddr=remoteAddr.sin_addr.s_addr;
////            pthread_mutex_lock(thMut);
////            thRemotaFD->insert(std::make_pair(tempRFD,reAddr));
////            thFdStatus->insert(std::make_pair(tempRFD,TCPLIVECOUNT));
////            pthread_mutex_unlock(thMut);
//            iter=thRemotaFD->begin();
//            enditer=thRemotaFD->end();
//            for (;iter!=enditer; ++iter) {
//                std::cout<<"ip:"<<iter->second<<std::endl;
//                //server send all ip
//                GNPacket tgcd;
//                tgcd.sysCode=SEND_IP;
//                tgcd.data=GUtils::itos(iter->second);
//                pthread_mutex_lock(thNetMut);
//                thTcps->sendData(tempRFD,&tgcd);
//                pthread_mutex_unlock(thNetMut);
//            }
//            pthread_mutex_lock(thMut);
//            thRemotaFD->insert(std::make_pair(tempRFD,reAddr));
//            thFdStatus->insert(std::make_pair(tempRFD,TCPLIVECOUNT));
//            pthread_mutex_unlock(thMut);
//            //server send name;
//            std::string tempn(th_obj->getLocalName());
//            GNPacket msg;
//            msg.sysCode=PLAYER_NAME;
//            msg.data=tempn;
//            pthread_mutex_lock(thNetMut);
//            thTcps->sendData(tempRFD,&msg);
//            pthread_mutex_unlock(thNetMut);
//        }
//    }
//    return NULL;
//}
//
//void* GNetServer::listenSRemotaServer(void* obj){
//    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
//    pthread_testcancel();
//    GNetServer *th_obj=(GNetServer *)obj;
//    TcpServer *thTcps=th_obj->tcps;
//    fd_set *thFDset=&(th_obj->rfdset);
//    pthread_mutex_t* thMut=&(th_obj->remoteFDIPMutex);
//    pthread_mutex_t* thNetMut=&(th_obj->netMutex);
//    std::map<int,unsigned int> *thRemotaFD=&(th_obj->remoteFDIP);
////    std::map<int,int> *thFdStatus=&(th_obj->fdStatusMap);
//    
////    int thLocalFD=th_obj->localFD;
//    int maxFD=0;
//    struct timeval tv;
//    std::map<int,unsigned int>::iterator iter;
//    std::map<int,unsigned int>::iterator enditer;
//    while (true) {
//        pthread_testcancel();
//        FD_ZERO(thFDset);
////        FD_SET(thLocalFD, thFDset);
////        maxFD=maxFD>thLocalFD?maxFD:thLocalFD;
//        iter=thRemotaFD->begin();
//        enditer=thRemotaFD->end();
//        for (; iter!=enditer; ++iter) {
//            FD_SET(iter->first, thFDset);
//            maxFD=maxFD>(iter->first)?maxFD:(iter->first);
//        }
//        tv.tv_sec=0;
//        tv.tv_usec=500;
//        int sel=select(maxFD+1, thFDset, NULL, NULL, &tv);
////        printf("===========================server============================\n");
//        if (sel<0) {
//            if (EINTR==errno) {
//                continue;
//            }else{
//                printf("select faild:%m");
//            }
//        }
//        iter=thRemotaFD->begin();
//        enditer=thRemotaFD->end();
//        while (iter!=enditer) {
//            if (FD_ISSET(iter->first, thFDset)){
//                GNPacket msg;
//                long lenr=thTcps->recvData(iter->first,&msg);
//                printf("Server Recv Content Packet:%d,%d,%s,%d,%s\n",msg.sysCode,msg.origin,msg.UUID.c_str(),msg.NPCode,msg.data.c_str());
//                if (lenr<=0) {
//                    close(iter->first);
//                    pthread_mutex_lock(thMut);
//                    thRemotaFD->erase(iter++);
//                    pthread_mutex_unlock(thMut);
////                    thFdStatus->find(iter->first)->second=0;
//                    //系统通知有连接断开
//                    GNPacket smsg;
//                    smsg.sysCode=DISCONNECTION;
//                    smsg.origin=msg.origin;
//                    th_obj->notificationSystemData(smsg);
//                }else{
//                    int tcd=msg.sysCode;
//                    if (REPLAYER_NAME==tcd) {
//                        //系统通知有新连接
//                        GNPacket smsg;
//                        smsg.sysCode=NEWCONNECTION;
//                        smsg.origin=msg.origin;
//                        smsg.data=msg.data;
//                        th_obj->notificationSystemData(smsg);
//                    }else{
//                        th_obj->distributeData(msg.UUID,msg);
//                    }
//                    iter++;
//                }
//            }else{
//                iter++;
//            }
//        }
//    }
//    return NULL;
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GNetServer::startSearchService(const char* uname){
    if (SERVER_STOP==serverStatus) {
        serverStatus=SERVER_C_RUN;
        localName=uname;
        udps=new UdpServer(52157,52156,true);
        if (udps->iniServer()) {
            pthread_create(&tidSearchServer,NULL,GNetServer::searchServer,this);
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
    pthread_mutex_t* tempmut=&(temp->udpMapMutex);
    std::map<int, int>* temproomlist=&(temp->udpMapStatus);
    std::map<int, int>::iterator itm;
    std::map<unsigned int, std::string>* temproomlistInfo=&(temp->udpMap);
    std::map<unsigned int, std::string>::iterator itminfo;
    while (true) {
        pthread_testcancel();
        char s='1';
        tempudp->sendMsg(&s);
        pthread_mutex_lock(tempmut);
        itm=temproomlist->begin();
        while (itm!=temproomlist->end()) {
            (itm->second)--;
            if (itm->second<=0) {
                std::cout<<"room:"<<itm->first<<" closed"<<std::endl;
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
    pthread_mutex_t* tempmut=&(tempgr->udpMapMutex);
    std::map<unsigned int, std::string>* temproomlistInfo=&(tempgr->udpMap);
    std::map<int, int>* temproomlist=&(tempgr->udpMapStatus);
    std::map<int, int>::iterator itm;
    while (true) {
        pthread_testcancel();
        char tbuffer[1024];
        sockaddr_in remoteRecAddr;
        long lenr=tempudps->recvMsg(tbuffer,1024,&remoteRecAddr);
        if (lenr>0) {
            std::string temprip=GUtils::cptos(inet_ntoa(remoteRecAddr.sin_addr));
            int rip=remoteRecAddr.sin_addr.s_addr;
            pthread_mutex_lock(tempmut);
            itm=temproomlist->find(rip);
            if (itm==temproomlist->end()) {
                temproomlist->insert(std::make_pair(rip,UDPLIVECOUNT));
                temproomlistInfo->insert(std::make_pair(rip,tbuffer));
                //搜索到一个UDP信号
            }else{
                itm->second=UDPLIVECOUNT;
            }
            pthread_mutex_unlock(tempmut);
        }
    }
    return NULL;
}

void GNetServer::connectService(int addr){
    if (SERVER_C_TCP_RUN!=serverStatus){
        serverStatus=SERVER_C_TCP_RUN;
        tcps=new TcpServer(52125);
        if ((localTcpFD=tcps->iniServer(10))>0) {
            int remoteFD=tcps->isConnect(addr, 52125);
            if (remoteFD>0) {
//                pthread_mutex_lock(&remoteFDIPMutex);
                remoteFDIP.insert(std::make_pair(remoteFD,addr));
                fdStatusMap.insert(std::make_pair(remoteFD, TCPLIVECOUNT));
//                pthread_mutex_unlock(&remoteFDIPMutex);
//                pthread_create(&tidListenCRemotaService,NULL,GNetServer::listenCRemotaServer,this);
//                pthread_create(&tidListenCConnectService,NULL,GNetServer::listenCConnectServer,this);
                pthread_create(&tidListenCConnectService,NULL,GNetServer::listenCNetService,this);
            }
        }
    }
}

void* GNetServer::listenCNetService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_testcancel();
    GNetServer *temp=(GNetServer *)obj;
    TcpServer *tempTcps=temp->tcps;
    int tempLocalFD=temp->localTcpFD;
    fd_set *temptRfdset=&(temp->rfdset);
    std::map<int,unsigned int> *tempRemotaFD=&(temp->remoteFDIP);
    std::map<int,int> *tempFdStatus=&(temp->fdStatusMap);
    int maxFD=0;
    struct timeval wt;
    while (true) {
        pthread_testcancel();
        FD_ZERO(temptRfdset);
        FD_SET(tempLocalFD, temptRfdset);
        maxFD=maxFD>tempLocalFD?maxFD:tempLocalFD;
        std::map<int,unsigned int>::iterator iter;
        for (iter=tempRemotaFD->begin(); iter!=tempRemotaFD->end(); ++iter) {
            FD_SET(iter->first, temptRfdset);
        }
        if (!tempRemotaFD->empty()) {
            --iter;
            maxFD=maxFD>(iter->first)?maxFD:(iter->first);
        }
        wt.tv_sec=0;
        wt.tv_usec=0;
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
            int tempRFD;
            if ((tempRFD=tempTcps->isAccept(&remoteAddr))>0) {
                unsigned int reAddr=remoteAddr.sin_addr.s_addr;
                tempRemotaFD->insert(std::make_pair(tempRFD,reAddr));
                
                std::string tempn(temp->getLocalName());
                GNPacket rmsg;
                rmsg.sysCode=PLAYER_NAME;
                rmsg.data=tempn;
                tempTcps->sendData(tempRFD,&rmsg);
            }
        }
        iter=tempRemotaFD->begin();
        while (iter!=tempRemotaFD->end()) {
            if (FD_ISSET(iter->first, temptRfdset)){
                GNPacket msg;
                long lenr=tempTcps->recvData(iter->first,&msg);
                printf("Client Recv Content Packet:%d,%d,%s,%d,%s\n",msg.sysCode,msg.origin,msg.UUID.c_str(),msg.NPCode,msg.data.c_str());
                if (lenr<=0) {
                    close(iter->first);
                    tempRemotaFD->erase(iter++);
                    //系统通知有连接断开
                    GNPacket smsg;
                    smsg.sysCode=DISCONNECTION;
                    smsg.origin=msg.origin;
                    temp->notificationSystemData(smsg);
                }else{
                    int tcd=msg.sysCode;
                    if (SEND_IP==tcd) {
                        int reip=GUtils::ctoi((msg.data).c_str());
                        printf("remota Msg:%s(--%d--)\n",(msg.data).c_str(),reip);
                        int tempremo=tempTcps->isConnect(reip, 52125);
                        if (tempremo>0) {
                            tempRemotaFD->insert(std::make_pair(tempremo,reip));
                            tempFdStatus->insert(std::make_pair(tempremo,TCPLIVECOUNT));
                        }
                    }else if (PLAYER_NAME==tcd) {
                        std::string tempn(temp->getLocalName());
                        GNPacket rmsg;
                        rmsg.sysCode=REPLAYER_NAME;
                        rmsg.data=tempn;
                        tempTcps->sendData(iter->first,&rmsg);
                        //系统通知有新连接
                        GNPacket smsg;
                        smsg.sysCode=NEWCONNECTION;
                        smsg.origin=msg.origin;
                        smsg.data=msg.data;
                        temp->notificationSystemData(smsg);
                    }else if (REPLAYER_NAME==tcd) {
                        //系统通知有新连接
                        GNPacket smsg;
                        smsg.sysCode=NEWCONNECTION;
                        smsg.origin=msg.origin;
                        smsg.data=msg.data;
                        temp->notificationSystemData(smsg);
                    }else{
                        temp->distributeData(msg.UUID,msg);
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

//void* GNetServer::listenCConnectServer(void* obj){
//    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
//    pthread_testcancel();
//    GNetServer *th_obj=(GNetServer *)obj;
//    TcpServer *thTcps=th_obj->tcps;
//    pthread_mutex_t* thMut=&(th_obj->remoteFDIPMutex);
//    std::map<int,unsigned int> *thRemotaFD=&(th_obj->remoteFDIP);
//    std::map<int,int> *thFdStatus=&(th_obj->fdStatusMap);
//    sockaddr_in remoteAddr;
//    int tempRFD;
//    if ((tempRFD=thTcps->isAccept(&remoteAddr))>0) {
//        unsigned int reAddr=remoteAddr.sin_addr.s_addr;
//        pthread_mutex_lock(thMut);
//        thRemotaFD->insert(std::make_pair(tempRFD,reAddr));
//        thFdStatus->insert(std::make_pair(tempRFD,TCPLIVECOUNT));
//        pthread_mutex_unlock(thMut);
//    }
//    return NULL;
//}
//void* GNetServer::listenCRemotaServer(void* obj){
//    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
//    pthread_testcancel();
//    GNetServer *th_obj=(GNetServer *)obj;
//    TcpServer *thTcps=th_obj->tcps;
//    pthread_mutex_t* thMut=&(th_obj->remoteFDIPMutex);
//    pthread_mutex_t* thNetMut=&(th_obj->netMutex);
//    fd_set *thFDset=&(th_obj->rfdset);
//    std::map<int,unsigned int> *thRemotaFD=&(th_obj->remoteFDIP);
//    std::map<int,int> *thFdStatus=&(th_obj->fdStatusMap);
//    int maxFD=0;
//    struct timeval tv;
//    std::map<int,unsigned int>::iterator iter;
//    std::map<int,unsigned int>::iterator enditer;
//    while (true) {
//        pthread_testcancel();
//        FD_ZERO(thFDset);
//        iter=thRemotaFD->begin();
//        enditer=thRemotaFD->end();
//        for (; iter!=enditer; ++iter) {
//            FD_SET(iter->first, thFDset);
//             maxFD=maxFD>(iter->first)?maxFD:(iter->first);
//        }
//        tv.tv_sec=0;
//        tv.tv_usec=500;
//        int sel=select(maxFD+1, thFDset, NULL, NULL, &tv);
////        printf("===========================client============================\n");
//        if (sel<0) {
//            if (EINTR==errno) {
//                continue;
//            }else{
//                printf("select faild:%m");
//            }
//        }
//        iter=thRemotaFD->begin();
//        enditer=thRemotaFD->end();
//        while (iter!=enditer) {
//            if (FD_ISSET(iter->first, thFDset)){
//                GNPacket msg;
//                long lenr=thTcps->recvData(iter->first,&msg);
//                printf("Client Recv Content Packet:%d,%d,%s,%d,%s\n",msg.sysCode,msg.origin,msg.UUID.c_str(),msg.NPCode,msg.data.c_str());
//                if (lenr<=0) {
//                    close(iter->first);
////                    thFdStatus->find(iter->first)->second=0;
//                    pthread_mutex_lock(thMut);
//                    thRemotaFD->erase(iter++);
//                    pthread_mutex_unlock(thMut);
//                    //系统通知有连接断开
//                    GNPacket smsg;
//                    smsg.sysCode=DISCONNECTION;
//                    smsg.origin=msg.origin;
//                    th_obj->notificationSystemData(smsg);
//                }else{
//                    int tcd=msg.sysCode;
//                    if (SEND_IP==tcd) {
//                        int reip=GUtils::stoi(msg.data);
//                        printf("remota Msg:%s(--%d--)\n",(msg.data).c_str(),reip);
//                        int tempremo=thTcps->isConnect(reip, 52125);
//                        if (tempremo>0) {
//                            pthread_mutex_lock(thMut);
//                            thRemotaFD->insert(std::make_pair(tempremo,reip));
//                            thFdStatus->insert(std::make_pair(tempremo,TCPLIVECOUNT));
//                            pthread_mutex_unlock(thMut);
//                        }
//                    }else if (PLAYER_NAME==tcd) {
//                        std::string tempn(th_obj->getLocalName());
//                        GNPacket rmsg;
//                        rmsg.sysCode=REPLAYER_NAME;
//                        rmsg.data=tempn;
//                        pthread_mutex_lock(thNetMut);
//                        thTcps->sendData(iter->first,&rmsg);
//                        pthread_mutex_unlock(thNetMut);
//                        //系统通知有新连接
//                        GNPacket smsg;
//                        smsg.sysCode=NEWCONNECTION;
//                        smsg.origin=msg.origin;
//                        smsg.data=msg.data;
//                        th_obj->notificationSystemData(smsg);
//                    }else if (REPLAYER_NAME==tcd) {
//                        //系统通知有新连接
//                        GNPacket smsg;
//                        smsg.sysCode=NEWCONNECTION;
//                        smsg.origin=msg.origin;
//                        smsg.data=msg.data;
//                        th_obj->notificationSystemData(smsg);
//                    }else{
//                        th_obj->distributeData(msg.UUID,msg);
//                    }
//                    iter++;
//                }
//            }else{
//                iter++;
//            }
//        }
//    }
//    return NULL;
//}

long GNetServer::sendNetPack(int fd,GNPacket* np){
//    pthread_mutex_unlock(&netMutex);
    return tcps->sendData(fd,np);
//    pthread_mutex_unlock(&netMutex);
}
long GNetServer::sendNetPack(GNPacket* np){
    long i=0;
    std::map<int,unsigned int>::iterator iter;
    for (iter=remoteFDIP.begin(); iter!=remoteFDIP.end(); ++iter) {
        i+=sendNetPack(iter->first, np);
    }
    return i;
}

void GNetServer::disconnectService(){
    if (SERVER_C_TCP_RUN==serverStatus){
        pthread_cancel(tidListenCConnectService);
//        pthread_cancel(tidListenCRemotaService);
        std::map<int,unsigned int>::iterator iter;
        for (iter=remoteFDIP.begin(); iter!=remoteFDIP.end(); ++iter) {
            close(iter->first);
        }
        remoteFDIP.clear();
        close(localTcpFD);
        delete tcps;
        serverStatus=SERVER_STOP;
    }
}