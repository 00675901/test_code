//
//  GSNotificationPool.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-22.
//
//

#include "GSNotificationPool.h"

static GSNotificationPool *gsnInstance;

GSNotificationPool::GSNotificationPool(void){}
GSNotificationPool::~GSNotificationPool(void){}

GSNotificationPool* GSNotificationPool::shareInstance(){
    if (!gsnInstance) {
        gsnInstance=new GSNotificationPool();
    }
    return gsnInstance;
}

void GSNotificationPool::postNotifications(){
    vector<NotiParams>::iterator iter=notifications.begin();
    while (iter!=notifications.end()) {
        CCNotificationCenter::sharedNotificationCenter()->postNotification(iter->name.c_str(), iter->obj);
        iter++;
    }
    notifications.clear();
}

void GSNotificationPool::postNotification(const char* name,CCObject *object){
    NotiParams tempa;
    tempa.name=name;
    (object==NULL)?tempa.obj=NULL:tempa.obj=object;
    notifications.push_back(tempa);
}