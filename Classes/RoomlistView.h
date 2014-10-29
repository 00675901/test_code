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
#include "RoomView.h"

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

class RoomlistView : public CCLayerColor{
private:
    CCLayerColor* roomListLayer;
    GRCServer* grc;
public:
    map<int,string> roomlist;
    set<int> roomFD;
    static CCScene* scene();
    RoomlistView();
    ~RoomlistView();
    CREATE_FUNC(RoomlistView);
    virtual bool init();
    void closeView();
    void updateRoomlist();
    void enterRoom(CCControlButton *sender);
};


#endif /* defined(__TestCocos2dx__RoomlistView__) */
