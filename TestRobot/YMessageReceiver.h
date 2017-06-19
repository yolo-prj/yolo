#ifndef _YOLO_MESSAGE_RECEIVER_H_
#define _YOLO_MESSAGE_RECEIVER_H_

#include <yolo/YFramework/YNetworkMessageListener.h>
#include <yolo/YFramework/YMessageFormatParser.h>
#include <yolo/YFramework/YMessage.h>

using namespace yolo;

class YCommandListener
{
public:
    virtual void onReceiveCommand(YMessage msg) = 0;
};

class YConfigListener
{
public:
    virtual void onReceiveConfig(YMessage msg) = 0;
};

class YCommandReceiver : public YNetworkMessageListener
{
private:
    YCommandReceiver(){}
public:
    YCommandReceiver(YCommandListener* listener, string msgformat);
    ~YCommandReceiver();
    virtual void onReceiveMessage(byte* data, uint length);

private:
    YMessageFormatParser*   parser;
    YCommandListener*	    _listener;
};


class YConfigReceiver : public YNetworkMessageListener
{
private:
    YConfigReceiver(){}
public:
    YConfigReceiver(YConfigListener* listener, string msgformat);
    ~YConfigReceiver();
    virtual void onReceiveMessage(byte* data, uint length);

private:
    YMessageFormatParser*   parser;
    YConfigListener*	    _listener;
};

#endif
