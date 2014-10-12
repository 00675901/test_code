//
//  RoomView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#include "RoomView.h"
#include "stdlib.h"

CCScene* RoomView::scene(int maxl){
    CCScene *scene = CCScene::create();
    RoomView *layer = RoomView::create(maxl);
    scene->addChild(layer);
    return scene;
}

RoomView::RoomView(int maxl){
    maxLinsten=maxl;
//    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 1, true);
    cout<<"RoomView BEGIN"<<endl;
}
RoomView::~RoomView(){
//    if (maxLinsten>0) {
        delete grs;
//    }else{
//        delete gcs;
//    }
    cout<<"RoomView END"<<endl;
}

//bool RoomView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
//    return true;
//}

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
    
    
//    CCLabelTTF* pLabels = CCLabelTTF::create("abcdefghijklmnopqrstuvwxyz123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Marker Felt", 30, CCSize(100, 300), kCCTextAlignmentLeft);
//    pLabels->setColor(ccc3(0,0,0));
//    pLabels->setAnchorPoint(ccp(0.5, 1));
//    pLabels->setPosition(ccp(this->getContentSize().width/2,this->getContentSize().height-60));
//    this->addChild(pLabels);
    
    clientLayer=CCLayerColor::create(ccc4(0, 0, 0, 255), this->getContentSize().width/5, this->getContentSize().height-60);
    clientLayer->setAnchorPoint(ccp(0, 0));
    clientLayer->setPosition(0, 0);
    this->addChild(clientLayer);
    
    msgLayer=CCLayerColor::create(ccc4(0, 0, 0, 255), this->getContentSize().width*4/5-10, this->getContentSize().height-60);
    msgLayer->setAnchorPoint(ccp(0, 0));
    msgLayer->setPosition(this->getContentSize().width/5+10, 0);
    this->addChild(msgLayer);
    
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomView::updateRoom), "updateRoom", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomView::updateMsglist), "updateMsg", NULL);
//    if (maxLinsten>0) {
        grs=new GRSServer(maxLinsten,&clientFD,&msglist);
        if (grs) {
            grs->init();
        }
//    }else{
//        gcs=new GRCServer();
//        if (gcs) {
//            gcs->init();
//        }
//    }
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance(), 0.5, false);
    cout<<"view init:"<<maxLinsten<<endl;
    return true;
}

void RoomView::updateRoom(){
    cout<<"client count:"<<clientFD.size()<<endl;
    clientLayer->removeAllChildren();
    set<int>::iterator iter=clientFD.begin();
    int i=0;
    while (iter!=clientFD.end()) {
        string ti="player ";
        ti.append(GUtils::itos(*iter));
//        cout<<ti<<endl;
        CCLabelTTF *ptext=CCLabelTTF::create(ti.c_str(), "Marker Felt", 30);
        ptext->setAnchorPoint(ccp(0.5,1));
        ptext->setPosition(ccp(clientLayer->getContentSize().width/2, clientLayer->getContentSize().height-(i*40)));
        clientLayer->addChild(ptext);
        iter++;
        i++;
    }
}

void RoomView::updateMsglist(){
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
//    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    clientFD.clear();
    msglist.clear();
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getScheduler()->unscheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance());
    pDirector->popScene();
//    this->removeFromParent();
}