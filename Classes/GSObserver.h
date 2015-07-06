//
//  GSObserver.h
//
#include "PackDefine.h"

class GSObserver{
public:
    GSObserver(){}
    virtual ~GSObserver(){}
    virtual void Update(TestPacket){}
};