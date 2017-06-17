#ifndef _YOLO_MESSAGE_RECEIVER_H_
#define _YOLO_MESSAGE_RECEIVER_H_

#include <yolo/YFramework/YNetworkMessageListener.h>
#include <yolo/YFramework/YMessageFormatParser.h>
#include <yolo/YFramework/YMessage.h>

using namespace yolo;

class YCommandReceiver : public YNetworkMessageListener
{
private:
    YCommandReceiver(){}
public:
    YCommandReceiver(string msgformat);
    ~YCommandReceiver();
    virtual void onReceiveMessage(byte* data, uint length);

private:
    YMessageFormatParser*   parser;
};


class YConfigReceiver : public YNetworkMessageListener
{
private:
    YConfigReceiver(){}
public:
    YConfigReceiver(string msgformat);
    ~YConfigReceiver();
    virtual void onReceiveMessage(byte* data, uint length);

private:
    YMessageFormatParser*   parser;
};

#endif
