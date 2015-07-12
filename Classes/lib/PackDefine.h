
#pragma once

#include <string>

////////////////////////////////////////////////////////////GNetServer使用数据包定义开始//////////////////////////////////////////////////////////////////////////////////////

//数据包
struct GNPacket{
    GNPacket():sysCode(-1),origin(-1),UUID("0"),NPCode(-1),data("0"){}
    int sysCode; //系统操作码 小于0:不转发 1:转发
    int origin;
    std::string UUID;
    int NPCode;	// 操作码
    std::string	data;
};
////数据包序列化
//static ByteBuffer& operator<<(ByteBuffer& lht, const GNPacket& rht){
//    lht<<rht.sysCode<<rht.origin<<rht.UUID<<rht.NPCode<<rht.data;
//    return lht;
//};
////数据包反序列化
//static ByteBuffer& operator>>(ByteBuffer& lht, GNPacket& rht){
//    lht>>rht.sysCode>>rht.origin>>rht.UUID>>rht.NPCode>>rht.data;
//    return lht;
//};
/////////////////////////////////////////////////////////////GNetServer使用数据包定义结束///////////////////////////////////////////////////////////////////////////////////////