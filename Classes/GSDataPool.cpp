//
//  GSNotificationPool.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-22.
//

#include "GSDataPool.h"

static GSDataPool *gsnInstance;

GSDataPool::GSDataPool(void){
    pthread_mutex_init(&mutex, NULL);
}
GSDataPool::~GSDataPool(void){
    pthread_mutex_destroy(&mutex);
}

GSDataPool* GSDataPool::shareInstance(){
    if (!gsnInstance) {
        gsnInstance=new GSDataPool();
    }
    return gsnInstance;
}

void GSDataPool::postData(std::string name,void* data){
    typedef std::pair<std::string,void*> tp;
    pthread_mutex_lock(&mutex);
    dataMap.insert(tp(name,data));
    pthread_mutex_unlock(&mutex);
}

void* GSDataPool::getData(std::string name){
    void* tempdata;
    pthread_mutex_lock(&mutex);
    tempdata=dataMap[name];
    pthread_mutex_unlock(&mutex);
    return tempdata;
}