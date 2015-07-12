//
//  GNetApplication.h
//  Cocos2dx
//  libuuid
//  gcc -o uuid uuid.c -luuid
//
#pragma once
#include "GNetServer.h"
#include <uuid/uuid.h>

class GNetApplications:public GNetObserver{
public:
    GNetApplications(){
        pthread_mutex_init(&mut, NULL);
        gns=GNetServer::shareInstance();
        std::cout<<"========================================="<<std::endl;
    }
    ~GNetApplications(){
        pthread_mutex_destroy(&mut);
    }
    pthread_mutex_t mut;
    std::string UDID;
    GNetServer* gns;
    bool bind(){
        uuid_t uuid;
        char s[40];
        uuid_generate(uuid);
        uuid_unparse_lower(uuid, s);
        std::string keys(s);
        UDID=keys;
        return gns->addObs(UDID, this);
    }
    virtual void NewConnection(GNPacket){}
    virtual void Update(GNPacket){}
    virtual void DisConnection(GNPacket){}
    
//    virtual void testUpdate(std::string){}
};

