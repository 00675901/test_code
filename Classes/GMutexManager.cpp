//
//  GMutexManager.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-22.
//
//

#include "GMutexManager.h"

GMutexManager::GMutexManager(pthread_mutex_t* mut){
    mutex=mut;
    pthread_mutex_init(mutex, NULL);
}
GMutexManager::~GMutexManager(){
    pthread_mutex_destroy(mutex);
}
