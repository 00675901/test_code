//
//  GSNotificationPool.h
//  TestCocos2dx
//
//  Created by othink on 14-8-22.
//
//

#ifndef __TestCocos2dx__GSNotificationPool__
#define __TestCocos2dx__GSNotificationPool__

#include <iostream>
#include <vector>
#include "cocos2d.h"
//#include "GMutexManager.h"

USING_NS_CC;
using namespace std;

class GSNotificationPool:public CCObject{
    pthread_mutex_t mutex;
    typedef struct{
        string name;
        CCObject* obj;
    }NotiParams;
    vector<NotiParams> notifications;
    GSNotificationPool(void);
    ~GSNotificationPool(void);
public:
    static GSNotificationPool* shareInstance();
    void postNotifications();
    void postNotification(const char* name,CCObject *object);
};

#endif /* defined(__TestCocos2dx__GSNotificationPool__) */
