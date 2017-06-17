#ifndef _YOLO_CONNECTION_LOST_LISTENER_H_
#define _YOLO_CONNECTION_LOST_LISTENER_H_

#include <yolo/YFramework/YMessage.h>


class YConnectionLostListener
{
    virtual void connectionLost() = 0;
};


#endif
