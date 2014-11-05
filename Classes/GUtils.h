//
//  GUtils.h
//  TestCocos2dx
//
//  Created by othink on 14-8-20.
//
//

#ifndef __TestCocos2dx__GUtils__
#define __TestCocos2dx__GUtils__

#include <iostream>
#include <sstream>

using namespace std;

class GUtils{
    
public:
    static int bytes2int(const char* bytes);
    static void int2bytes(int i,unsigned char* bytes);
    static string itos(int i);
    static string cptos(char* s);
    static const char* itoc(int i);
    static int ctoi(const char* c);
};

#endif /* defined(__TestCocos2dx__GUtils__) */
