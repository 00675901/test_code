//
//  RoomView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#include "RoomView.h"

RoomView::RoomView(){
    FD_ZERO(&cfds);
    pthread_mutex_init(&mut, NULL);
    cout<<"view begin"<<endl;
}
RoomView::~RoomView(){
    FD_ZERO(&cfds);
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
    fd_set *tempCfds=&(temp->cfds);
    set<int> *tempClientDF=&(temp->clientFD);
    int tempTcpsSocket=temp->tcpsSocket;
    int res;
    int maxFD=0;
    while (true) {
        FD_ZERO(tempCfds);
        FD_SET(tempTcpsSocket, tempCfds);
        maxFD=maxFD>(tempTcpsSocket)?maxFD+1:tempTcpsSocket+1;
//        set<int>::iterator iter=tempClientDF->begin();
//        while (iter!=tempClientDF->end()) {
//            FD_SET(*iter, tempCfds);
//        }
//        if (!tempClientDF->empty()) {
//            --iter;
//            maxFD=maxFD>*iter?maxFD+1:(*iter)+1;
//        }
        int sel=select(maxFD, tempCfds, NULL, NULL, NULL);
        if (sel<0) {
            cout<<"select error"<<endl;
            continue;
        }
        if (FD_ISSET(tempTcpsSocket, tempCfds)) {
            
            cout<<"1"<<endl;
            if ((res=tempTcps->isAccept())>0) {
                char tt[]="welcome to room";
                tempTcps->sendMsg(res,tt,strlen(tt));
                tempClientDF->insert(res);
            }
        }else{
            cout<<"2"<<endl;
        }
        cout<<"3"<<endl;
        
//        GSNotificationPool::shareInstance()->postNotification("testPthread", NULL);
//        GSNotificationPool::shareInstance()->postNotification("updateRoom", NULL);
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