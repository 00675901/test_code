//
//  RoomlistView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-10-9.
//
//

#include "RoomlistView.h"

CCScene* RoomlistView::scene(){
    CCScene *scene = CCScene::create();
    RoomlistView *layer = RoomlistView::create();
    scene->addChild(layer);
    return scene;
}
RoomlistView::RoomlistView(){
    cout<<"RoomlistView BEGIN"<<endl;
}
RoomlistView::~RoomlistView(){
    delete grc;
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
    
    grc=new GRCServer(&roomFD,&roomName);
    if (grc) {
        grc->init();
    }
    
    cout<<"RoomlistView INIT"<<endl;
    return true;
}

void RoomlistView::closeView(){
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "updateRoomList");
    CCDirector::sharedDirector()->popScene();
}

void RoomlistView::updateRoomlist(){
    cout<<"updata room FD:"<<roomFD.size()<<endl;
    cout<<"updata room NAME:"<<roomName.size()<<endl;
}
