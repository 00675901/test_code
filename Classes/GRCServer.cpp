//
//  GRCServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRCServer.h"

GRCServer::GRCServer(){
    cout<<"Client Server service BEGIN"<<endl;
}
GRCServer::~GRCServer(){
    cout<<"Client Server service END"<<endl;
}
bool GRCServer::init(){
    return true;
}
