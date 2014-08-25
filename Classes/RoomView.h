//
//  RoomView.h
//  TestCocos2dx
//
//  Created by othink on 14-8-25.
//
//

#ifndef __TestCocos2dx__RoomView__
#define __TestCocos2dx__RoomView__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

class RoomView : public CCLayerColor{
public:
    RoomView();
    ~RoomView();
    virtual bool init();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    CREATE_FUNC(RoomView);
};

#endif /* defined(__TestCocos2dx__RoomView__) */
