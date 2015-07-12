
#pragma once

#include <string>
#include "ByteBuffer.h"

////////////////////////////////////////////////////////////GNetServer使用数据包定义开始//////////////////////////////////////////////////////////////////////////////////////

#define GNP_SPC ":*:"

//数据包
struct GNPacket{
    GNPacket():sysCode(-1),origin(-1),UUID("0"),NPCode(-1),data("0"){}
    int sysCode; //系统操作码 小于0:不转发 1:转发
    int origin;
    int NPCode;	// 操作码
    std::string UUID;
    std::string	data;
    
    int size(){
        std::string a=GNP_SPC;
        int i=0;
        i+=GUtils::itos(this->sysCode).length();
        i+=GUtils::itos(this->origin).length();
        i+=GUtils::itos(this->NPCode).length();
        i+=this->UUID.length();
        i+=this->data.length();
        i+=strlen(a.c_str())*4;
        return i;
    }
    void serializer(char* container){
        std::string a=GNP_SPC;
        const char* bas=a.c_str();
        sprintf(container, "%d%s%d%s%d%s%s%s%s",this->sysCode,bas,this->origin,bas,this->NPCode,bas,this->UUID.c_str(),bas,this->data.c_str());
    }
    GNPacket& deserializer(char* content){
        std::vector<char *> va;
        std::string a=GNP_SPC;
        const char* sff=a.c_str();
        char *tempp=strtok(content, sff);
        while(tempp!=NULL){
            va.push_back(tempp);
            tempp=strtok(NULL, sff);
        }
        this->sysCode=GUtils::stoi(va[0]);
        this->origin=GUtils::stoi(va[1]);
        this->NPCode=GUtils::stoi(va[2]);
        std::string s1(va[3]);
        std::string s2(va[4]);
        this->UUID=s1;
        this->data=s2;
        return *this;
    }
};
//数据包序列化
static ByteBuffer& operator<<(ByteBuffer& lht, const GNPacket& rht){
    lht<<rht.sysCode<<rht.origin<<rht.UUID<<rht.NPCode<<rht.data;
    return lht;
};
//数据包反序列化
static ByteBuffer& operator>>(ByteBuffer& lht, GNPacket& rht){
    lht>>rht.sysCode>>rht.origin>>rht.UUID>>rht.NPCode>>rht.data;
    return lht;
};
/////////////////////////////////////////////////////////////GNetServer使用数据包定义结束///////////////////////////////////////////////////////////////////////////////////////