//
//  RoomList.h
//  TestCocos2dx
//  Created by GgYyer on 14-8-17.
//
//

#ifndef __TestCocos2dx__RoomList__
#define __TestCocos2dx__RoomList__

#include <iostream>
#include <vector>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "RoomView.h"
#include "RoomlistView.h"
#include "CCTextField.h"

#include "PackDefine.h"

USING_NS_CC_EXT;
USING_NS_CC;

#define USER_NAME_FIELD_DEFAULT "User Name"

using namespace std;

class RoomManager : public CCLayer
{
private:
    CCLayerColor *listView;
    CCControlButton *createBtn;
    CCControlButton *joinBtn;
    CCTextField *userNameField;
public:
    static CCScene* scene();
    RoomManager();
    ~RoomManager();
    virtual bool init();
    CREATE_FUNC(RoomManager);
    void createRoom();
    void joinRoom();
};

#endif /* defined(__TestCocos2dx__RoomList__) */
