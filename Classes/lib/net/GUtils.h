//
//  GUtils.h
//  Cocos2dx
//

#ifndef __GUtils__
#define __GUtils__

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

typedef int8_t int8;

class GUtils{
public:
    static int bytes2int(const char* bytes);
    static void int2bytes(int i,unsigned char* bytes);
    static std::string itos(int i);
    static int stoi(std::string s);
    static std::string ltos(long l);
    static std::string cptos(char* s);
    static int ctoi(const char* c);
};

#endif /* defined(__GUtils__) */
