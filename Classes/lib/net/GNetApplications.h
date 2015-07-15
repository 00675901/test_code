//
//  GNetApplication.h
//  Cocos2dx
//  libuuid
//  gcc -o uuid uuid.c -luuid
//
#pragma once
#include "GNetServer.h"

#define CLASS_NAME(_TYPE_) typeid(_TYPE_).name()

class GNetApplications:public GNetObserver{
public:
    GNetApplications(){
        pthread_mutex_init(&mut, NULL);
        gns=GNetServer::shareInstance();
    }
    ~GNetApplications(){
        pthread_mutex_destroy(&mut);
    }
    pthread_mutex_t mut;
    std::string UDID;
    GNetServer* gns;
    bool bind(const char* cn){
        std::string keys(cn);
        UDID=keys;
        return gns->addObs(UDID, this);
    }
    virtual void NewConnection(GNPacket){}
    virtual void Update(GNPacket){}
    virtual void DisConnection(GNPacket){}
    
//    virtual void testUpdate(std::string){}
};

