//
//  GRServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14/11/10.
//
//

#include "GNServer.h"

GNServer::GNServer(void){
    FD_ZERO(&rfdset);
    pthread_mutex_init(&mut, NULL);
    printf("GNServer BEGIN\n");
}
GNServer::~GNServer(void){
    FD_ZERO(&rfdset);
    pthread_mutex_destroy(&mut);
    printf("GNServer END\n");
}

unsigned int GNServer::getLocalIP(){
    return localIP;
}

map<int,unsigned int> GNServer::getRemoteFDIP(){
    return remoteFDIP;
}

void* GNServer::listenNetService(void* obj){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_testcancel();
    GNServer *temp=(GNServer *)obj;
    TcpServer *tempTcps=temp->tcps;
    int tempLocalFD=temp->localTcpFD;
    fd_set *temptRfdset=&(temp->rfdset);
    map<int,unsigned int> *tempRemotaFD=&(temp->remoteFDIP);
    ByteBuffer tempbb=temp->bb;
    int res;
    int maxFD=0;
    typedef pair<int,unsigned int> tp;
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
                TestPacket msg;
                char* pDataBuffer;
                int lenr=tempTcps->recvData(iter->first,pDataBuffer);
                // 清除缓存中数据
                tempbb.clear();
                // 将数据加入到缓存中去
                tempbb.append((uint8_t*)pDataBuffer, sizeof(pDataBuffer));
                tempbb>>msg;
                if (lenr<=0) {
                    close(iter->first);
                    tempRemotaFD->erase(iter++);
                }else{
//                    string topc=msg.code;
//                    string tdat=msg.data;
//                    if (topc.compare(GNOC_SIP)==0) {
//                        int reip=GUtils::ctoi(tdat.c_str());
//                        printf("remota Msg:%s(--%d--)\n",tdat.c_str(),reip);
//                        int tempremo=tempTcps->isConnect(reip, 52125);
//                        if (tempremo>0) {
//                            tempRemotaFD->insert(tp(tempremo,reip));
//                        }
//                    }else if (topc.compare(GNOC_GIP)==0){
//                        map<int,unsigned int>::iterator iters;
//                        for (iters=tempRemotaFD->begin(); iters!=tempRemotaFD->end(); ++iters) {
//                            msg.code=GNOC_SIP;
//                            msg.data=iters->second;
//                            tempbb<<msg;
//                            tempTcps->sendData(iters->first,(char*)tempbb.contents());
//                        }
//                    }
                    temp->notify(msg);
                    iter++;
                }
//                GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
//                GSNotificationPool::shareInstance()->postNotification("updateMsg", NULL);
            }else{
                iter++;
            }
        }
    }
    return NULL;
}