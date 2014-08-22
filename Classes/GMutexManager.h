//
//  GMutexManager.h
//  TestCocos2dx
//
//  Created by othink on 14-8-22.
//
//

#ifndef __TestCocos2dx__GMutexManager__
#define __TestCocos2dx__GMutexManager__

#include <iostream>

class GMutexManager{
    pthread_mutex_t* mutex;
public:
    GMutexManager(pthread_mutex_t* mut);
    ~GMutexManager();
};

#endif /* defined(__TestCocos2dx__GMutexManager__) */
