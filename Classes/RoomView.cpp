//
//  RoomView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#include "RoomView.h"
#include "stdlib.h"

RoomView::RoomView(int maxl){
    maxLinsten=maxl;
    FD_ZERO(&rfdset);
    FD_ZERO(&wfdset);
    FD_ZERO(&efdset);
    pthread_mutex_init(&mut, NULL);
    cout<<"view begin"<<endl;
}
RoomView::~RoomView(){
    FD_ZERO(&rfdset);
    FD_ZERO(&wfdset);
    FD_ZERO(&efdset);
    pthread_mutex_destroy(&mut);
    delete udps;
    delete tcps;
    cout<<"view end"<<endl;
}

bool RoomView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    return true;
}

bool RoomView::init(){
    cout<<"init begin:"<<maxLinsten<<endl;
    
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
    
    
    CCLabelTTF* pLabels = CCLabelTTF::create("abcdefghijklmnopqrstuvwxyz123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Marker Felt", 30, CCSize(100, 300), kCCTextAlignmentLeft);
    pLabels->setColor(ccc3(0,0,0));
    pLabels->setAnchorPoint(ccp(0.5, 1));
    pLabels->setPosition(ccp(this->getContentSize().width/2,this->getContentSize().height-60));
    this->addChild(pLabels);
    
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
    if ((tcpsSocket=tcps->iniServer(maxLinsten))>0) {
        pthread_create(&tidtcp,NULL,RoomView::listenRoomService,this);
    }
}

void RoomView::updateRoom(CCObject *param){
    clientLayer->removeAllChildren();
    cout<<"update room:"<<clientFD.size()<<endl;
    set<int>::iterator iter=clientFD.begin();
    int i=0;
    while (iter!=clientFD.end()) {
        string ti="player ";
        ti.append(GUtils::itos(*iter));
        cout<<ti<<endl;
        CCLabelTTF *ptext=CCLabelTTF::create(ti.c_str(), "Marker Felt", 30);
        ptext->setAnchorPoint(ccp(0.5,1));
        ptext->setPosition(ccp(clientLayer->getContentSize().width/2, clientLayer->getContentSize().height-(i*40)));
        clientLayer->addChild(ptext);
        iter++;
        i++;
    }
}

void RoomView::updateMsglist(CCObject* param){
//    const char* dss=(const char*)param;
//    string s(dss);
    string* s=(string*)param;
    cout<<"testADDs2:"<<s<<endl;
    cout<<"testADDs2_:"<<&s<<endl;
    cout<<"ss char count:"<<s->length()<<"\nss char:"<<*s<<endl;
    msglist.push_back(*s);
    delete s;
    if (msglist.size()>14) {
        msglist.pop_front();
    }
    cout<<"msg count:"<<msglist.size()<<endl;
    msgLayer->removeAllChildren();
    deque<string>::iterator iter=msglist.begin();
    int i=0;
    while (iter!=msglist.end()) {
//        string ti="player ";
//        ti.append(GUtils::itos(*iter));
        cout<<"strin:"<<iter->c_str()<<endl;
        CCLabelTTF *ptext=CCLabelTTF::create(iter->c_str(), "Marker Felt", 30);
        ptext->setAnchorPoint(ccp(0,1));
        ptext->setPosition(ccp(5, msgLayer->getContentSize().height-(i*40)));
        msgLayer->addChild(ptext);
        iter++;
        i++;
    }
}

void* RoomView::listenRoomService(void* obj){
    RoomView *temp=(RoomView *)obj;
    TcpServer *tempTcps=temp->tcps;
    fd_set *tempRfdset=&(temp->rfdset);
    fd_set *tempWfdset=&(temp->wfdset);
    fd_set *tempEfdset=&(temp->efdset);
    set<int> *tempClientFD=&(temp->clientFD);
//    deque<string> *tempMsglist=&(temp->msglist);
    int tempTcpsSocket=temp->tcpsSocket;
    int res;
    int maxFD=0;
    while (true) {
        FD_ZERO(tempRfdset);
        FD_ZERO(tempWfdset);
        FD_ZERO(tempEfdset);
        FD_SET(tempTcpsSocket, tempRfdset);
        FD_SET(tempTcpsSocket, tempEfdset);
        maxFD=maxFD>tempTcpsSocket?maxFD:tempTcpsSocket;
        set<int>::iterator iter;
        for (iter=tempClientFD->begin(); iter!=tempClientFD->end(); ++iter) {
            FD_SET(*iter, tempRfdset);
            FD_SET(*iter, tempWfdset);
            FD_SET(*iter, tempEfdset);
        }
        if (!tempClientFD->empty()) {
            --iter;
            maxFD=maxFD>*iter?maxFD:*iter;
        }
        int sel=select(maxFD+1, tempRfdset, tempWfdset, tempEfdset, NULL);
        if (sel<0) {
            if (EINTR==errno) {
                continue;
            }else{
                printf("select faild:%m");
            }
        }
        if (FD_ISSET(tempTcpsSocket, tempRfdset)) {
            if ((res=tempTcps->isAccept())>0) {
                tempClientFD->insert(res);
//                string *ts=new string("a player join the room!");
                string ts="a player leave the room";
//                cout<<"testADDs1:"<<ts<<endl;
//                cout<<"testADDs1_:"<<&ts<<endl;
//                CCString ts=CCString("a player join the room!\n");
                GSNotificationPool::shareInstance()->postNotification("updateRoom",NULL);
//                GSNotificationPool::shareInstance()->postNotification("updateMsg",(CCObject *)ts);
                GSNotificationPool::shareInstance()->postNotification("updateMsg",ts);
//                tempMsglist->push_back(ts);
//                if (tempMsglist->size()>14) {
//                    tempMsglist->pop_front();
//                }
                const char* temps=ts.c_str();
//                const char* temps=ts->c_str();
                cout<<"char count:"<<strlen(temps)<<"\nchar:"<<temps<<endl;
                for (iter=tempClientFD->begin(); iter!=tempClientFD->end(); ++iter) {
                    tempTcps->sendMsg(*iter,temps,strlen(temps));
                }
            }
        }
        iter=tempClientFD->begin();
        while (iter!=tempClientFD->end()) {
            if (FD_ISSET(*iter, tempRfdset)){
                char tt[8];
                int lenr=tempTcps->recvMsg(*iter,tt,8);
                if (lenr<=0) {
                    close(*iter);
                    string ttt="a player leave the room";
                    cout<<ttt<<endl;
                    tempClientFD->erase(iter++);
                    temp->sendMsgToAll(ttt.c_str());
//                    tempMsglist->push_back(ttt);
//                    if (tempMsglist->size()>14) {
//                        tempMsglist->pop_front();
//                    }
                    GSNotificationPool::shareInstance()->postNotification("updateRoom",NULL);
//                    GSNotificationPool::shareInstance()->postNotification("updateMsg", (CCObject *)&ttt);
                }else{
                    tempTcps->sendMsg(*iter,tt,8);
                    iter++;
                }
            }else{
                iter++;
            }
        }
    }
    return NULL;
}

void* RoomView::sendRoomService(void* obj){
    UdpServer *temp=(UdpServer *)obj;
    while (true) {
        char s[]="this is udp msg";
        temp->sendMsg(s, strlen(s));
        sleep(1);
    }
    return NULL;
}

void RoomView::sendMsgToAll(const char* msg){
    int msgsize=strlen(msg);
    cout<<"Msg Count:"<<msgsize<<endl;
    set<int>::iterator iters=clientFD.begin();
    while (iters!=clientFD.end()) {
        tcps->sendMsg(*iters, msg, msgsize);
        iters++;
    }
}

void RoomView::closeView(){
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "updateRoom");
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(GSNotificationPool::postNotifications), GSNotificationPool::shareInstance());
    pthread_cancel(tidudp);
    pthread_cancel(tidtcp);
    clientFD.clear();
    this->removeFromParent();
}