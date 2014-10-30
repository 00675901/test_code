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
    return true;
}

void RoomManager::createRoom(){
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCScene* pScene=RoomView::scene(10);
    pDirector->pushScene(pScene);
}

void RoomManager::joinRoom(){
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCScene* pScene=RoomlistView::scene();
    pDirector->pushScene(pScene);
}