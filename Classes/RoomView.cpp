//
//  RoomView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#include "RoomView.h"

RoomView::RoomView(){
    cout<<"view begin"<<endl;
}
RoomView::~RoomView(){
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
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 1, true);
    roomServer();
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance(), 0.5, false);
    cout<<"view inin"<<endl;
    return true;
}

void RoomView::roomServer(){
    udps=new UdpServer(50002,50003);
    if (udps->iniServer()) {
        pthread_create(&tidudp,NULL,RoomView::sendRoomService,udps);
    }
    tcps=new TcpServer(50001);
    if (tcps->iniServer(10)) {
        pthread_create(&tidtcp,NULL,RoomView::listenRoomService,tcps);
    }
}

void RoomView::updateRoom(){
    cout<<"update room"<<endl;
}

void* RoomView::listenRoomService(void* obj){
    TcpServer *temp=(TcpServer *)obj;
    while (temp->isAccept()) {
        char tt[]="welcome to room";
        temp->sendMsg(tt, strlen(tt));
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
    this->removeFromParent();
}