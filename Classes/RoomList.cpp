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
    
    mainID=pthread_self();
    
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(RoomList::testPool), this, 1.0/60.0, false);
    return true;
}

void RoomList::createRoom(){
    createListView();
//    for (int i=1; i<10; i++) {
//        CCControlButton *button = CCControlButton::create("Quit", "Marker Felt", 30);
//        button->setTitleColorForState(ccBLACK, CCControlStateNormal);
//        button->setTitleColorForState(ccRED, CCControlStateHighlighted);
//        button->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomList::closeListView), CCControlEventTouchUpInside);
//        button->setAnchorPoint(ccp(0,1));
//        button->setPosition(20, listView->getContentSize().height-(i*40));
//        listView->addChild(button);
//    }
    printf("create.\n");
}
void RoomList::joinRoom(){
//    createListView();
    CCLog("joinRoom--");
    if(mainID==pthread_self()){
        CCLog("In Main Pthread");
    }else{
        CCLog("In Chirld Pthread");
    }
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomList::recvClient), "recvclient", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomList::testPthread), "testtest", NULL);
    
    tcps=new TcpServer(12345);
    if (tcps->iniServer(10)) {
        pthread_t tid;
        pthread_create(&tid,NULL,RoomList::listenRoom,tcps);
    }
}

void* RoomList::listenRoom(void* obj){
    TcpServer *temp=(TcpServer *)obj;
    while (true) {
        temp->isAccept();
        CCNotificationCenter::sharedNotificationCenter()->postNotification("recvclient");
    }
    return NULL;
}

void RoomList::testPool(){
    int t=gtpool.size();
    for (int i=0; i<t; i++) {
        CCNotificationCenter::sharedNotificationCenter()->postNotification(gtpool[i].c_str());
    }
    gtpool.clear();
}

void RoomList::recvClient(){
    CCLog("recvClient--");
    if(mainID==pthread_self()){
        CCLog("In Main Pthread");
    }else{
        CCLog("In Chirld Pthread");
    }
//    pthread_mutex_lock(gmut);
    gtpool.push_back("testtest");
//    pthread_mutex_unlock(gmut);
}

void RoomList::testPthread(){
    CCLog("testPthread--");
    if(mainID==pthread_self()){
        CCLog("In Main Pthread");
    }else{
        CCLog("In Chirld Pthread");
    }
}

void* RoomList::recvClientService(void* obj){
    return NULL;
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
    pthread_mutex_destroy(gmut);
    delete tcps;
    this->removeChild(listView);
    createBtn->setTouchEnabled(true);
    joinBtn->setTouchEnabled(true);
}