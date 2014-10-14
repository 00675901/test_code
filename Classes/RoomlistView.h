//
//  RoomlistView.h
//  TestCocos2dx
//
//  Created by othink on 14-10-9.
//
//

#ifndef __TestCocos2dx__RoomlistView__
#define __TestCocos2dx__RoomlistView__

#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "GSNotificationPool.h"
#include "GRCServer.h"

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

class RoomlistView : public CCLayerColor{
private:
    CCLayerColor* roomListLayer;
    GRCServer* grc;
public:
    map<string,string> roomAddr;
    deque<int> roomFD;
    deque<string> roomName;
    static CCScene* scene();
    RoomlistView();
    ~RoomlistView();
    CREATE_FUNC(RoomlistView);
    virtual bool init();
    void closeView();
    void updateRoomlist();
};

#endif /* defined(__TestCocos2dx__RoomlistView__) */