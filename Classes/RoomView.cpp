//
//  RoomView.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#include "RoomView.h"

bool RoomView::init(){
    // 1. super init first
    if (!CCLayerColor::initWithColor(ccc4(255, 255, 255, 255))){
        return false;
    }
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128,true);
    cout<<"view inin"<<endl;
    return true;
}

RoomView::RoomView(){
    cout<<"view begin"<<endl;
}
RoomView::~RoomView(){
    cout<<"view end"<<endl;
}

bool RoomView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    cout<<"view touch"<<endl;
    return true;
}

