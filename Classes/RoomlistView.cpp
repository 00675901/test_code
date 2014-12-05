//
//  RoomlistView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-10-9.
//
//

#include "RoomlistView.h"

CCScene* RoomlistView::scene(const char* username){
    CCScene *scene = CCScene::create();
    RoomlistView *layer = RoomlistView::create(username);
    scene->addChild(layer);
    return scene;
}
RoomlistView::RoomlistView(const char* username){
    uname=username;
    cout<<"RoomlistView BEGIN"<<endl;
}
RoomlistView::~RoomlistView(){
    grc=GRCServer::shareInstance();
    if (grc) {
        grc->stopFindRoomService();
    }
    roomlist.clear();
    cout<<"RoomlistView END"<<endl;
}
bool RoomlistView::init(){
    if (!CCLayerColor::initWithColor(ccc4(255, 255, 255, 255))){
        return false;
    }
    CCControlButton* closeButton = CCControlButton::create("Quit", "Marker Felt", 30);
    closeButton->setTitleColorForState(ccBLACK, CCControlStateNormal);
    closeButton->setTitleColorForState(ccRED, CCControlStateHighlighted);
    closeButton->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomlistView::closeView), CCControlEventTouchUpInside);
    closeButton->setAnchorPoint(ccp(1,1));
    closeButton->setPosition(this->getContentSize().width-20, this->getContentSize().height-20);
    this->addChild(closeButton);
    CCLabelTTF* pLabel = CCLabelTTF::create("Room List", "Marker Felt", 30);
    pLabel->setColor(ccc3(0,0,0));
    pLabel->setAnchorPoint(ccp(0.5, 1));
    pLabel->setPosition(ccp(this->getContentSize().width/2,this->getContentSize().height-20));
    this->addChild(pLabel);
    roomListLayer=CCLayerColor::create(ccc4(0, 0, 0, 255), this->getContentSize().width, this->getContentSize().height-60);
    roomListLayer->setAnchorPoint(ccp(0, 0));
    roomListLayer->setPosition(0, 0);
    this->addChild(roomListLayer);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomlistView::updateRoomlist), "updateRoomList", NULL);
    grc=GRCServer::shareInstance();
    if (grc) {
        grc->startFindRoomService(&roomlist);
    }
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance(), 0.5, false);
    cout<<"RoomlistView INIT"<<endl;
    return true;
}

void RoomlistView::closeView(){
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "updateRoomList");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getScheduler()->unscheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance());
    pDirector->popScene();
}

void RoomlistView::updateRoomlist(){
    roomListLayer->removeAllChildren();
    map<int, string>::iterator iter=roomlist.begin();
    int i=1;
    while (iter!=roomlist.end()) {
        printf("\nIP:%d,name:%s\n\n",iter->first,iter->second.c_str());
        CCControlButton *pbtn=CCControlButton::create(iter->second.c_str(), "Marker Felt", 50);
        pbtn->setAnchorPoint(ccp(0.5,1));
        pbtn->setPosition(ccp(roomListLayer->getContentSize().width/2, roomListLayer->getContentSize().height-(i*80)));
        pbtn->setTitleColorForState(ccWHITE, CCControlStateNormal);
        pbtn->setTitleColorForState(ccRED, CCControlStateHighlighted);
        pbtn->setTag(iter->first);
        pbtn->addTargetWithActionForControlEvents(this, cccontrol_selector(RoomlistView::enterRoom), CCControlEventTouchUpInside);
        roomListLayer->addChild(pbtn);
        i++;
        iter++;
    }
}

void RoomlistView::enterRoom(CCControlButton *sender){
    printf("tag:%d\n",sender->getTag());
    if(grc->initConnectService(sender->getTag())){
        CCDirector* pDirector = CCDirector::sharedDirector();
        CCScene* pScene=RoomView::scene(10,false,uname);
        pDirector->pushScene(pScene);
    }
}
