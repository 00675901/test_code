//
//  RoomView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#include "RoomView.h"
#include "stdlib.h"

CCScene* RoomView::scene(int maxl,bool isServer,const char* uname){
    CCScene *scene = CCScene::create();
    RoomView *layer = RoomView::create(maxl,isServer,uname);
    scene->addChild(layer);
    return scene;
}

RoomView::RoomView(int maxl,bool isServer,const char* username){
    maxLinsten=maxl;
    isSer=isServer;
    uname=username;
    cout<<"RoomView BEGIN"<<endl;
}
RoomView::~RoomView(){
    cout<<"RoomView END"<<endl;
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

    clientLayer=CCLayerColor::create(ccc4(0, 0, 0, 255), this->getContentSize().width/5, this->getContentSize().height-60);
    clientLayer->setAnchorPoint(ccp(0, 0));
    clientLayer->setPosition(0, 0);
    this->addChild(clientLayer);
    CCLabelTTF *ptext=CCLabelTTF::create(uname, "Marker Felt", 30);
    ptext->setAnchorPoint(ccp(0.5,1));
    ptext->setPosition(ccp(clientLayer->getContentSize().width/2, clientLayer->getContentSize().height));
    this->addChild(ptext);
    
    msgLayer=CCLayerColor::create(ccc4(0, 0, 0, 255), this->getContentSize().width*4/5-10, this->getContentSize().height-60);
    msgLayer->setAnchorPoint(ccp(0, 0));
    msgLayer->setPosition(this->getContentSize().width/5+10, 0);
    this->addChild(msgLayer);
    
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomView::updateRoom), "updateRoom", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomView::updateMsglist), "updateMsg", NULL);
    if (isSer) {
        grs=GNSServer::shareInstance();
        if (grs) {
//            grs->startListenRoomService(maxLinsten,uname);
        }
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance(), 0.5, false);
    }else{
        gcs=GNCServer::shareInstance();
        if (gcs) {
           
        }
    }
    cout<<"view init:"<<maxLinsten<<endl;
    return true;
}

void RoomView::updateRoom(){
    map<int,string> clientFD;
    if (isSer) {
//        clientFD=grs->getRemoteFDName();
    }else{
//        clientFD=gcs->getRemoteFDName();
    }
    cout<<"client count:"<<clientFD.size()<<endl;
    clientLayer->removeAllChildren();
    map<int,string>::iterator iter=clientFD.begin();
    int i=1;
    while (iter!=clientFD.end()) {
        string ti=iter->second;
        CCLabelTTF *ptext=CCLabelTTF::create(ti.c_str(), "Marker Felt", 30);
        ptext->setAnchorPoint(ccp(0.5,1));
        ptext->setPosition(ccp(clientLayer->getContentSize().width/2, clientLayer->getContentSize().height-(i*40)));
        clientLayer->addChild(ptext);
        iter++;
        i++;
    }
}

void RoomView::updateMsglist(){
    deque<string> msglist;
    if (isSer) {
//        msglist=grs->getLoglist();
    }else{
//        msglist=gcs->getLoglist();
    }
    cout<<"msg count:"<<msglist.size()<<endl;
    msgLayer->removeAllChildren();
    for (int i=0; i<msglist.size(); i++) {
        CCLabelTTF *ptext=CCLabelTTF::create(msglist[i].c_str(), "Marker Felt", 30);
        ptext->setAnchorPoint(ccp(0,1));
        ptext->setPosition(ccp(5, msgLayer->getContentSize().height-(i*40)));
        msgLayer->addChild(ptext);
    }
}

void RoomView::closeView(){
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "updateRoom");
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "updateMsg");
    CCDirector* pDirector = CCDirector::sharedDirector();
    if (isSer) {
        pDirector->getScheduler()->unscheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance());
        if(grs){
            
        }
    }else{
        if (gcs) {
            
        }
    }
    pDirector->popScene();
}