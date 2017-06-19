#ifndef _YOLO_NETWORK_MESSAGE_LISTENER_
#define _YOLO_NETWORK_MESSAGE_LISTENER_

#include <yolo/YFramework/YMessage.h>

class YNetworkMessageListener
{
public:
    virtual void onReceiveMessage(byte* data, uint length) = 0;
    virtual void onTcpClientConnected(int handle, string addr, ushort port) {} ;
    virtual void onTcpClientDisconnected(int handle, string addr, ushort port) {} ;
};


#endif
