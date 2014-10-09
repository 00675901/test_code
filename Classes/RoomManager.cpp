//
//  RoomList.cpp
//  TestCocos2dx
//
//  Created by GgYyer on 14-8-17.
//
//

#include "RoomManager.h"

CCScene* RoomManager::scene(){
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    // 'layer' is an autorelease object
    RoomManager *layer = RoomManager::create();
    // add layer as a child to scene
    scene->addChild(layer);
    // return the scene
    return scene;
}

RoomManager::RoomManager(){
    cout<<"RoomManager BEGIN"<<endl;
}
RoomManager::~RoomManager(){
    cout<<"RoomManager END"<<endl;
}

// on "init" you need to initialize your instance
bool RoomManager::init(){
    // 1. super init first
    if (!CCLayer::init()){
        return false;
    }
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
//    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    
    createBtn = CCControlButton::create("Create Game","Marker Felt", 50);
    createBtn->setTitleColorForState(ccWHITE, CCControlStateNormal);
    createBtn->setTitleColorForState(ccRED, CCControlStateHighlighted);
    createBtn->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomManager::createRoom), CCControlEventTouchUpInside);
    createBtn->setAnchorPoint(ccp(0.5,0.5));
    createBtn->setPosition(visibleSize.width/2, visibleSize.height/2+50);
    this->addChild(createBtn);
    joinBtn = CCControlButton::create("Join Game", "Marker Felt", 50);
    joinBtn->setTitleColorForState(ccWHITE, CCControlStateNormal);
    joinBtn->setTitleColorForState(ccRED, CCControlStateHighlighted);
    joinBtn->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomManager::joinRoom), CCControlEventTouchUpInside);
    joinBtn->setAnchorPoint(ccp(0.5,0.5));
    joinBtn->setPosition(visibleSize.width/2, visibleSize.height/2-50);
    this->addChild(joinBtn);
    
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance(), 0.5, false);
    return true;
}

void RoomManager::createRoom(){

//    RoomView* rv=RoomView::create(10);
//    this->addChild(rv);
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCScene* pScene=RoomView::scene(10);
    pDirector->pushScene(pScene);
//    pDirector->replaceScene(pScene);
//    pDirector->runWithScene(pScene);
//    udps=new UdpServer(40001,40002);
//    if (udps->iniServer()) {
//        pthread_t tid;
//        pthread_create(&tid,NULL,RoomManager::sendRoom,udps);
////        pthread_t tids;
////        pthread_create(&tids,NULL,RoomList::listenRoom,udps);
//    }
//    createListView();
    printf("create.\n");
}

void RoomManager::joinRoom(){
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCScene* pScene=RoomlistView::scene();
    pDirector->pushScene(pScene);
    
//    udps=new UdpServer(40001,40002);
//    if (udps->iniServer()) {
//        pthread_t tid;
//        pthread_create(&tid,NULL,RoomManager::sendRoom,udps);
//    }
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

void* RoomManager::sendRoom(void* obj){
    UdpServer *temp=(UdpServer *)obj;
    while (true) {
        char s[]="this is udp msg";
        temp->sendMsg(s, strlen(s));
        sleep(3);
    }
    return NULL;
}

void* RoomManager::listenRoom(void* obj){
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
        while (true) {
            rec=temp->recvMsg(s,len);
            cout<<"char count:"<<strlen(s)<<endl;
            contents.append(s, rec);
            cout<<"count:"<<rec<<endl;
            //        GSNotificationPool::shareInstance()->postNotification("recvclient", NULL);
            cout<<"recvUDP:"<<contents<<endl;
            sockaddr_in* sss=temp->getRemoteRecAddr();
            cout<<"recvUDP addr:"<<inet_ntoa(sss->sin_addr)<<endl;
            cout<<"recvUDP port:"<<sss->sin_port<<endl;
            contents.clear();
        }
    
//    }
    return NULL;
}

void RoomManager::recvClient(){
}

void RoomManager::createListView(){
    createBtn->setTouchEnabled(false);
    joinBtn->setTouchEnabled(false);
    listView=CCLayerColor::create(ccc4(255, 255, 255, 255));
    listView->setContentSize(CCDirector::sharedDirector()->getWinSize());
    this->addChild(listView);
    CCControlButton *button = CCControlButton::create("Quit", "Marker Felt", 30);
    button->setTitleColorForState(ccBLACK, CCControlStateNormal);
    button->setTitleColorForState(ccRED, CCControlStateHighlighted);
    button->setAnchorPoint(ccp(1,1));
    button->setPosition(listView->getContentSize().width-20, listView->getContentSize().height-20);
    listView->addChild(button);
    
    
}

