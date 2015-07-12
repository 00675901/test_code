//
//  ByteBuffer.h
//  NetWorkFrame
//

#pragma once

#include <vector>
#include "GUtils.h"

class ByteBuffer{
protected:
    size_t                  BRPos;
    size_t                  BWPos;
    std::vector<uint8_t>	BData;
    
public:
    ByteBuffer(): BRPos(0),BWPos(0){}
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
    void append(const ByteBuffer& buffer){
        if (buffer.size()) append(buffer.contents(),buffer.size());
    }
    void append(const uint8_t *src, size_t cnt)
    {
        if (!cnt) return;
//        assert(size() < 10000000);
        if (BData.size() < BWPos + cnt){
            BData.resize(BWPos + cnt);
        }
        memcpy(&BData[BWPos], src, cnt);
        BWPos += cnt;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T> T read()
    {
        T r = read<T>(BRPos);
        BRPos += sizeof(T);
        return r;
    };
    template <typename T> T read(size_t pos) const
    {
//        assert(pos + sizeof(T) <= size() || PrintPosError(false,pos,sizeof(T)));
        return *((T const*)&BData[pos]);
    }
    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    ByteBuffer& operator<<(bool value){
        append<char>((char)value);
        return *this;
    }
    ByteBuffer& operator<<(uint8_t value){
        append<uint8_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(uint16_t value){
        append<uint16_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(uint32_t value){
        append<uint32_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(uint64_t value){
        append<uint64_t>(value);
        return *this;
    }
    
    ByteBuffer& operator<<(int8_t value){
        append<int8_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(int16_t value){
        append<int16_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(int32_t value){
        append<int32_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(int64_t value){
        append<int64_t>(value);
        return *this;
    }
    
    ByteBuffer& operator<<(float value){
        append<float>(value);
        return *this;
    }
    ByteBuffer& operator<<(double value){
        append<double>(value);
        return *this;
    }
    ByteBuffer& operator<<(time_t value){
        append<time_t>(value);
        return *this;
    }
    
    ByteBuffer& operator<<(const std::string& value){
        append((uint8_t const *)value.c_str(), value.length());
        append((uint8_t)0);
        return *this;
    }
    ByteBuffer& operator<<(const char* str){
        append( (uint8_t const *)str, str ? strlen(str) : 0);
        append((uint8_t)0);
        return *this;
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    ByteBuffer& operator>>(bool& value){
        value = read<char>() > 0 ? true : false;
        return *this;
    }
    ByteBuffer& operator>>(uint8_t& value){
        value = read<uint8_t>();
        return *this;
    }
    ByteBuffer& operator>>(uint16_t& value){
        value = read<uint16_t>();
        return *this;
    }
    ByteBuffer& operator>>(uint32_t& value){
        value = read<uint32_t>();
        return *this;
    }
    ByteBuffer& operator>>(uint64_t& value){
        value = read<uint64_t>();
        return *this;
    }
    
    ByteBuffer& operator>>(int8_t& value){
        value = read<int8_t>();
        return *this;
    }
    ByteBuffer& operator>>(int16_t& value){
        value = read<int16_t>();
        return *this;
    }
    ByteBuffer& operator>>(int32_t& value){
        value = read<int32_t>();
        return *this;
    }
    ByteBuffer& operator>>(int64_t& value){
        value = read<int64_t>();
        return *this;
    }
    
    ByteBuffer& operator>>(float& value){
        value = read<float>();
        return *this;
    }
    ByteBuffer& operator>>(double& value){
        value = read<double>();
        return *this;
    }
    ByteBuffer& operator>>(time_t& value){
        value = read<time_t>();
        return *this;
    }
    ByteBuffer& operator>>(std::string& value){
        value.clear();
        while (BRPos < size()){
            char c = read<char>();
            if (c == 0){
                break;
            }
            value += c;
        }
        return *this;
    }
    ByteBuffer& operator>>(char value[]){
        std::string strValue;
        strValue.clear();
        while (BRPos < size()){
            char c = read<char>();
            if (c == 0){
                break;
            }
            strValue += c;
        }
        strncpy(value, strValue.c_str(), strValue.size());
        return *this;
    }
};
