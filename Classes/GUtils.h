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

class GUtils{
    
public:
    static int bytes2int(const char* bytes);
    static void int2bytes(int i,unsigned char* bytes);

};

#endif /* defined(__TestCocos2dx__GUtils__) */
