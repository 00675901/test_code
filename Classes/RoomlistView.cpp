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
    roomlist->clear();
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
//    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RoomlistView::updateRoomlist), "updateRoomList", NULL);
    
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(RoomlistView::updateRoomlist), this, 1, false);
    
    //启动客户端
    std::string tempna(uname);
    gnapp=NetAppCCJSController::shareInstance(tempna);
    gnapp->start_client();
    roomlist=gnapp->getServerList();
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance(), 0.5, false);
    cout<<"RoomlistView INIT"<<endl;
    return true;
}

void RoomlistView::closeView(){
//    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "updateRoomList");
    gnapp->stop_client_service();
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getScheduler()->unscheduleSelector(schedule_selector(RoomlistView::updateRoomlist), this);
    pDirector->getScheduler()->unscheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance());
    pDirector->popScene();
}

void RoomlistView::updateRoomlist(){
    
    cout<<"server_JSON:"<<gnapp->get_server_list()<<endl;
    
    roomlist=gnapp->getServerList();
    roomListLayer->removeAllChildren();
    map<unsigned int, string>::iterator iter=roomlist->begin();
    int i=1;
    while (iter!=roomlist->end()) {
        printf("IP:%d,name:%s\n",iter->first,iter->second.c_str());
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
    //暂停客户端
    gnapp->pause_client_service();
    //连接到服务器
    gnapp->connect_server(sender->getTag());
    
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCScene* pScene=RoomView::scene(10,false,uname);
    pDirector->pushScene(pScene);
}
