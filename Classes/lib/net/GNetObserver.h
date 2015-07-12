//
//  GSObserver.h
//
#include "PackDefine.h"

class GNetObserver{
public:
    GNetObserver(){}
    virtual ~GNetObserver(){}
    virtual void NewConnection(GNPacket)=0;
    virtual void Update(GNPacket)=0;
    virtual void DisConnection(GNPacket)=0;
//    virtual void testUpdate(std::string)=0;
};