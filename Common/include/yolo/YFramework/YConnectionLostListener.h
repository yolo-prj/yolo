#ifndef _YOLO_CONNECTION_LOST_LISTENER_H_
#define _YOLO_CONNECTION_LOST_LISTENER_H_

#include <yolo/YFramework/YMessage.h>


class YConnectionLostListener
{
public:
    virtual void connectionLost(string addr, ushort port) = 0;
};


#endif
