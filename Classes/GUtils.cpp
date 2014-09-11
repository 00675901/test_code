//
//  Utils.cpp
//  TestCocos2dx
//
//  Created by othink on 14-8-20.
//
//

#include "GUtils.h"

int GUtils::bytes2int(const char* bytes){
    return atoi(bytes);
}

void GUtils::int2bytes(int i,unsigned char* bytes){
    
}

string GUtils::itos(int i){
    stringstream s;
    s<<i;
    return s.str();
}
