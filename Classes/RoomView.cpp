//
//  RoomView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#include "RoomView.h"
#include <sys/socket.h>
#include <arpa/inet.h>

RoomView::RoomView(){
    FD_ZERO(&rfdset);
    FD_ZERO(&wfdset);
    FD_ZERO(&efdset);
    pthread_mutex_init(&mut, NULL);
    cout<<"view begin"<<endl;
}
RoomView::~RoomView(){
    FD_ZERO(&rfdset);
    FD_ZERO(&wfdset);
    FD_ZERO(&efdset);
    pthread_mutex_destroy(&mut);
    delete udps;
    delete tcps;
    cout<<"view end"<<endl;
}

bool RoomView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    return true;
}

bool RoomView::init(){
    // 1. super init first
    if (!CCLayerColor::initWithColor(ccc4(255, 255, 255, 255))){
        return false;
    }
    CCControlButton* closeButton = CCControlButton::create("Quit", "Marker Felt", 30);
    closeButton->setTitleColorForState(ccBLACK, CCControlStateNormal);
    closeButton->setTitleColorForState(ccRED, CCControlStateHighlighted);
    closeButton->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomView::closeView), CCControlEventTouchUpInside);
    closeButton->setAnchorPoint(ccp(1,1));
    closeButton->setPosition(this->getContentSize().width-20, this->getContentSize().height-20);
    this->addChild(closeButton);
    CCLabelTTF* pLabel = CCLabelTTF::create("waitting for player", "Marker Felt", 30);
    pLabel->setColor(ccc3(0,0,0));
    pLabel->setAnchorPoint(ccp(0.5, 1));
    pLabel->setPosition(ccp(this->getContentSize().width/2,this->getContentSize().height-20));
    this->addChild(pLabel);
    
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomView::updateRoom), "updateRoom", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomView::testPthread), "testPthread", NULL);
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 1, true);
    roomServer();
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance(), 0.5, false);
    cout<<"view inin"<<endl;
    return true;
}

void RoomView::testPthread(){
    cout<<"clientDF_Count:"<<this->clientFD.size()<<endl;
}

void RoomView::roomServer(){
    udps=new UdpServer(50002,50003);
    if (udps->iniServer()) {
        pthread_create(&tidudp,NULL,RoomView::sendRoomService,udps);
    }
    tcps=new TcpServer(50001);
    if ((tcpsSocket=tcps->iniServer(10))>0) {
        pthread_create(&tidtcp,NULL,RoomView::listenRoomService,this);
    }
}

void RoomView::updateRoom(){
    cout<<"update room"<<endl;
}

void* RoomView::listenRoomService(void* obj){
    RoomView *temp=(RoomView *)obj;
    TcpServer *tempTcps=temp->tcps;
    fd_set *tempRfdset=&(temp->rfdset);
    fd_set *tempWfdset=&(temp->wfdset);
    fd_set *tempEfdset=&(temp->efdset);
    set<int> *tempClientDF=&(temp->clientFD);
    int tempTcpsSocket=temp->tcpsSocket;
    int res;
    int maxFD=0;
    while (true) {
//        set<int>::iterator iter;
//        if ((res=tempTcps->isAccept())>0) {
//            tempClientDF->insert(res);
//            GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
//            for (iter=tempClientDF->begin(); iter!=tempClientDF->end(); ++iter) {
//                char tt[]="a player join the room!\n";
//                send(*iter,tt,strlen(tt),0);
//                tempTcps->sendMsg(*iter,tt,strlen(tt));
//                tempTcps->sendMsg(res,tt,strlen(tt));
//            }
//        }
        FD_ZERO(tempRfdset);
        FD_ZERO(tempWfdset);
        FD_ZERO(tempEfdset);
        FD_SET(tempTcpsSocket, tempRfdset);
        FD_SET(tempTcpsSocket, tempEfdset);
        maxFD=maxFD>tempTcpsSocket?maxFD:tempTcpsSocket;
        set<int>::iterator iter;
        for (iter=tempClientDF->begin(); iter!=tempClientDF->end(); ++iter) {
            FD_SET(*iter, tempRfdset);
            FD_SET(*iter, tempWfdset);
            FD_SET(*iter, tempEfdset);
        }
        if (!tempClientDF->empty()) {
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
                tempClientDF->insert(res);
                GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
                for (iter=tempClientDF->begin(); iter!=tempClientDF->end(); ++iter) {
                    char tt[]="a player join the room!\n";
                    tempTcps->sendMsg(*iter,tt,strlen(tt));
                }
            }
        }else{
            for (iter=tempClientDF->begin(); iter!=tempClientDF->end(); ++iter) {
                if (FD_ISSET(*iter, tempRfdset)){
                    char tt[8];
                    int lenr=tempTcps->recvMsg(*iter,tt,8);
                    if (lenr<=0) {
                        close(*iter);
                        
                        cout<<"a player leave the room"<<endl;
                    }else{
                        tempTcps->sendMsg(*iter,tt,8);
                    }
                }
            }
        }
        
//        GSNotificationPool::shareInstance()->postNotification("testPthread", NULL);
    }
    return NULL;
}

void* RoomView::sendRoomService(void* obj){
    UdpServer *temp=(UdpServer *)obj;
    while (true) {
        char s[]="this is udp msg";
        temp->sendMsg(s, strlen(s));
        sleep(3);
    }
    return NULL;
}

void RoomView::closeView(){
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "updateRoom");
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance());
    pthread_cancel(tidudp);
    pthread_cancel(tidtcp);
    clientFD.clear();
    this->removeFromParent();
}