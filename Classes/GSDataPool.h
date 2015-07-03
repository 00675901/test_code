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
#include <map>

class GSDataPool{
private:
    pthread_mutex_t mutex;
    GSDataPool(void);
    ~GSDataPool(void);
    std::map<std::string,void*> dataMap;
public:
    static GSDataPool* shareInstance();
    void postData(std::string,void*);
    void* getData(std::string);
};

#endif /* defined(__TestCocos2dx__GSNotificationPool__) */
