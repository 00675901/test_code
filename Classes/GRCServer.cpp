//
//  GRCServer.cpp
//  TestCocos2dx
//
//  Created by othink on 14-9-29.
//
//

#include "GRCServer.h"

GRCServer::GRCServer(){
    cout<<"Client Server service Begin"<<endl;
}
GRCServer::~GRCServer(){
    cout<<"Client Server service End"<<endl;
}
bool GRCServer::init(){
    return true;
}
