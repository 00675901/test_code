//
//  ByteBuffer.h
//  NetWorkFrame
//

#pragma once

#include <vector>
#include "GUtils.h"
#include "PackDefine.h"

class ByteBuffer{
protected:
    size_t                  BRPos;
    size_t                  BWPos;
    std::vector<uint8_t>	BData;
public:
    ByteBuffer(): BRPos(0),BWPos(0){BData.reserve(512);}
    ByteBuffer(size_t res): BRPos(0),BWPos(0){
        BData.reserve(res);
    }
    ByteBuffer(const ByteBuffer &buf):BRPos(buf.BRPos),BWPos(buf.BWPos),BData(buf.BData){}
    ~ByteBuffer(){}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    const uint8_t* contents() const { return &BData[BRPos]; }
    
    size_t size() const { return BData.size(); }
    
    bool empty() const { return BData.empty(); }
    
    void clear(){
        BData.clear();
        BRPos = BWPos = 0;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    void append(T value){
        append((uint8_t*)&value, sizeof(value));
    }
    void append(const uint8_t *src, size_t cnt){
        if (!cnt) return;
        if (BData.size() < BWPos + cnt){
            BData.resize(BWPos + cnt);
        }
        memcpy(&BData[BWPos], src, cnt);
        BWPos += cnt;
    }
    
    void appendn(const std::string& value){
        append((uint8_t const *)value.c_str(), value.length());
        append((uint8_t)0);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T> T read(){
        T r = read<T>(BRPos);
        BRPos += sizeof(T);
        return r;
    };
    template <typename T> T read(size_t pos) const{
//        assert(pos + sizeof(T) <= size() || PrintPosError(false,pos,sizeof(T)));
        return *((T const*)&BData[pos]);
    }
    
    void read(uint8_t *dest, size_t len){
//        assert(BRPos  + len  <= size() || PrintPosError(false, BRPos,len));
        memcpy(dest, &BData[BRPos], len);
        BRPos += len;
    }
    
    void readn(std::string& value){
        value.clear();
        while (BRPos < size()){
            char c = read<char>();
            if (c == 0){
                break;
            }
            value += c;
        }
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ByteBuffer& operator<<(GNPacket* gp){
        append((uint8_t*)&(gp->sysCode),sizeof(gp->sysCode));
        append((uint8_t*)&(gp->origin),sizeof(gp->origin));
        appendn(gp->UUID);
        append((uint8_t*)&(gp->NPCode),sizeof(gp->NPCode));
        appendn(gp->data);
        return *this;
    }
    ByteBuffer& operator>>(GNPacket* gp){
        read((uint8_t*)&(gp->sysCode),sizeof(gp->sysCode));
        read((uint8_t*)&(gp->origin),sizeof(gp->origin));
        readn(gp->UUID);
        read((uint8_t*)&(gp->NPCode),sizeof(gp->NPCode));
        readn(gp->data);
        return *this;
    }
    
    ByteBuffer& operator<<(std::string* gp){
        return *this;
    }
    
    ByteBuffer& operator>>(std::string* gp){
        return *this;
    }
    
};
