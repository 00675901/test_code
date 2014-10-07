//
//  GSNotificationPool.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-22.
//

#include "GSNotificationPool.h"

static GSNotificationPool *gsnInstance;

GSNotificationPool::GSNotificationPool(void){
    pthread_mutex_init(&mutex, NULL);
}
GSNotificationPool::~GSNotificationPool(void){
    pthread_mutex_destroy(&mutex);
}

GSNotificationPool* GSNotificationPool::shareInstance(){
    if (!gsnInstance) {
        gsnInstance=new GSNotificationPool();
    }
    return gsnInstance;
}

void GSNotificationPool::postNotifications(){
    pthread_mutex_lock(&mutex);
    vector<NotiParams>::iterator iter=notifications.begin();
    while (iter!=notifications.end()) {
        CCNotificationCenter::sharedNotificationCenter()->postNotification(iter->name.c_str(), iter->obj);
        iter++;
    }
    notifications.clear();
    pthread_mutex_unlock(&mutex);
}

void GSNotificationPool::postNotification(const char* name,string object){
    pthread_mutex_lock(&mutex);
    NotiParams tempa;
    tempa.name=name;
    (object=="")?tempa.obj=NULL:tempa.obj=(CCObject *)&object;
    notifications.push_back(tempa);
    pthread_mutex_unlock(&mutex);
}

void GSNotificationPool::postNotification(const char* name,CCObject *object){
    pthread_mutex_lock(&mutex);
    NotiParams tempa;
    tempa.name=name;
    (object==NULL)?tempa.obj=NULL:tempa.obj=object;
    notifications.push_back(tempa);
    pthread_mutex_unlock(&mutex);
}
