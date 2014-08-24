//
//  RoomList.cpp
//  TestCocos2dx
//
//  Created by GgYyer on 14-8-17.
//
//

#include "RoomList.h"

CCScene* RoomList::scene(){
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    // 'layer' is an autorelease object
    RoomList *layer = RoomList::create();
    // add layer as a child to scene
    scene->addChild(layer);
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool RoomList::init(){
    // 1. super init first
    if (!CCLayer::init()){
        return false;
    }
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
//    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    createBtn = CCControlButton::create("Create Game","Marker Felt", 30);
    createBtn->setTitleColorForState(ccWHITE, CCControlStateNormal);
    createBtn->setTitleColorForState(ccRED, CCControlStateHighlighted);
    createBtn->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomList::createRoom), CCControlEventTouchUpInside);
    createBtn->setAnchorPoint(ccp(0,1));
    createBtn->setPosition(20, visibleSize.height-20);
    this->addChild(createBtn);
    
    joinBtn = CCControlButton::create("Join Game", "Marker Felt", 30);
    joinBtn->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomList::joinRoom), CCControlEventTouchUpInside);
    joinBtn->setAnchorPoint(ccp(0,1));
    joinBtn->setPosition(20, visibleSize.height-70);
    this->addChild(joinBtn);
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance(), 0.5, false);
    return true;
}

void RoomList::createRoom(){
    udps=new UdpServer(40001,40002);
    
//    udps->iniServer();
//    int len=8;
//    char s[len];
//    string contents;
//    while (true) {
//        udps->recvMsg(s,len);
//        contents.append(s, strlen(s));
//        cout<<"recvUDP:"<<contents<<endl;
//        sockaddr_in* sss=udps->getRemoteRecAddr();
//        cout<<"recvUDP addr:"<<inet_ntoa(sss->sin_addr)<<endl;
//        //        GSNotificationPool::shareInstance()->postNotification("recvclient", NULL);
//    }

    if (udps->iniServer()) {
//        pthread_t tid;
//        pthread_create(&tid,NULL,RoomList::sendRoom,udps);
        pthread_t tids;
        pthread_create(&tids,NULL,RoomList::listenRoom,udps);
    }
//    createListView();
    printf("create.\n");
}

void RoomList::joinRoom(){
//    createListView();
//    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomList::recvClient), "recvclient", NULL);
//    tcps=new TcpServer(12345);
//    if (tcps->iniServer(10)) {
//        pthread_t tid;
//        pthread_create(&tid,NULL,RoomList::listenRoom,tcps);
//    }
//    if (tcps->isConnect()) {
//        pthread_t tid;
//        pthread_create(&tid,NULL,RoomList::listenRoom,tcps);
//    }
}

void* RoomList::sendRoom(void* obj){
    UdpServer *temp=(UdpServer *)obj;
    while (true) {
        char s[]="aaaaaaa";
        temp->sendMsg(s, strlen(s));
        sleep(3);
    }
    return NULL;
}

void* RoomList::listenRoom(void* obj){
//    TcpServer *temp=(TcpServer *)obj;
//    char tt[]="testtest_testtest";
//    temp->sendMsg(tt, strlen(tt));
//    while (temp->isAccept()) {
//        char tt[]="testtest_testtest";
//        temp->sendMsg(tt, strlen(tt));
//    }
    UdpServer *temp=(UdpServer *)obj;
//    while (true) {
        int len=8;
        char s[len];
        string contents;
        int rec;
        while ((rec=temp->recvMsg(s,len))>0) {
            cout<<"char count:"<<strlen(s)<<endl;
            contents.append(s, strlen(s));
            cout<<"count:"<<rec<<endl;
            //        GSNotificationPool::shareInstance()->postNotification("recvclient", NULL);
        }
        cout<<"recvUDP:"<<contents<<endl;
        sockaddr_in* sss=temp->getRemoteRecAddr();
        cout<<"recvUDP addr:"<<inet_ntoa(sss->sin_addr)<<endl;
//    }
    return NULL;
}

void RoomList::recvClient(){
}

void RoomList::createListView(){
    createBtn->setTouchEnabled(false);
    joinBtn->setTouchEnabled(false);
    listView=CCLayerColor::create(ccc4(255, 255, 255, 255));
    listView->setContentSize(CCDirector::sharedDirector()->getWinSize());
    this->addChild(listView);
    CCControlButton *button = CCControlButton::create("Quit", "Marker Felt", 30);
    button->setTitleColorForState(ccBLACK, CCControlStateNormal);
    button->setTitleColorForState(ccRED, CCControlStateHighlighted);
    button->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomList::closeListView), CCControlEventTouchUpInside);
    button->setAnchorPoint(ccp(1,1));
    button->setPosition(listView->getContentSize().width-20, listView->getContentSize().height-20);
    listView->addChild(button);
}

void RoomList::closeListView(){
    delete tcps;
    delete udps;
    this->removeChild(listView);
    createBtn->setTouchEnabled(true);
    joinBtn->setTouchEnabled(true);
}