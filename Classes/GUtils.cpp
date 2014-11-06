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

string GUtils::cptos(char* s){
    string ss(s);
    return ss;
}

int GUtils::ctoi(const char* c){
    int r;
    stringstream ss;
    ss<<c;
    ss>>r;
    return r;
}
