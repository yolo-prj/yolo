#ifndef _YOLO_MESSAGE_SENDER_H_
#define _YOLO_MESSAGE_SENDER_H_

#include <yolo/YFramework/YMessage.h>
#include <yolo/YFramework/YNetworkManager.h>

using namespace yolo;

class YImageSender
{
private:
    YImageSender(){}
public:
    YImageSender(YNetworkManager*);
    ~YImageSender();
    void send(YMessage& msg);

private:
    YNetworkManager*	manager;
};

class YEventSender
{
private:
    YEventSender(){}
public:
    YEventSender(YNetworkManager*);
    ~YEventSender();
    void send(YMessage& msg);
    
private:
    YNetworkManager*	manager;
};

#endif
