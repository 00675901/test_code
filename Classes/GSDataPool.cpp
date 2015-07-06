//
//  GSNotificationPool.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-22.
//

#include "GSDataPool.h"

void GSDataPool::Update(TestPacket a){
    printf("code=%s,data=%s",a.code.c_str(),a.data.c_str());
}