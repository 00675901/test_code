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
    
    userNameField=CCTextField::textFieldWithPlaceHolder(USER_NAME_FIELD_DEFAULT,"", 50.0f);
    string a="测试玩家No.";
    string b(GUtils::itos((arc4random()%100)+10));
    a.append(b);
    userNameField->setString(a.c_str());
    userNameField->setDimensions(CCSizeMake(400, 50));
    userNameField->setPosition(ccp(this->getContentSize().width/2,this->getContentSize().height-100));
    userNameField->setAnchorPoint(ccp(0.5,1));
    this->addChild(userNameField);
    
    
//    ByteBuffer bbs;
    GNPacket gns;
    gns.sysCode=234;
    gns.origin=5435;
    gns.NPCode=-6578;
    gns.UUID="afefsfeggsdg";
    gns.data="文dgeg地方噶尔+）——（adfs";
    
//    cout<<"cionsdf:"<<gns.size()<<endl;
    
    char s[gns.size()];
//    memcpy(s, (uint8_t *)&gns.sysCode, sizeof(int));
    
    gns.serializer(s);
    
    cout<<"cionsdf:"<<s<<" siezes:"<<sizeof(s)<<endl;
    
    GNPacket gsss;
    gsss.deserializer(s);
    
    printf("vvvvv:%d,%d,%s,%d,%s\n",gsss.sysCode,gsss.origin,gsss.UUID.c_str(),gsss.NPCode,gsss.data.c_str());
    
//    std::string gns("\n");
    
//    cout<<"cionsdf:"<<gns.size()<<" coanfsdfa:"<<sizeof(gns)<<" sdfae:"<<gns.length()<<endl;
    
//    cout<<"cionsdf:"<<gns.size()<<" coanfsdfa:"<<sizeof(gns)<<" sdfae:"<<gns.length()<<endl;;
    
//    bbs<<gns;
//    
//    cout<<"sasdfgas:"<<(char*)bbs.contents()<<endl;
//    
//    GNPacket gnsss;
//    bbs>>gnsss;
//    printf("vvvvv:%d,%d,%s,%d,%s",gnsss.sysCode,gnsss.origin,gnsss.UUID.c_str(),gnsss.NPCode,gnsss.data.c_str());
    return true;
}

void RoomManager::createRoom(){
    const char* username=userNameField->getString();
    cout<<"username:"<<username<<endl;    
    if(strlen(username)!=0){
        CCDirector* pDirector = CCDirector::sharedDirector();
        CCScene* pScene=RoomView::scene(10,true,username);
        pDirector->pushScene(pScene);
    }else{
        printf("Input your user name please!\n");
    }
}

void RoomManager::joinRoom(){
    const char* username=userNameField->getString();
    if(strlen(username)!=0){
        CCDirector* pDirector = CCDirector::sharedDirector();
        CCScene* pScene=RoomlistView::scene(username);
        pDirector->pushScene(pScene);
    }else{
        printf("Input your user name please!\n");
    }
}