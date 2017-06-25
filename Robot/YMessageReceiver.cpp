#include "YMessageReceiver.h"

YCommandReceiver::YCommandReceiver(YCommandListener* listener, string msgformat)
{
    _listener = listener;
    parser = new YMessageFormatParser(msgformat);
    parser->parse();
}

YCommandReceiver::~YCommandReceiver()
{
    delete parser;
}

void
YCommandReceiver::onReceiveMessage(byte* data, uint length)
{
    uint opcode;
    memcpy(&opcode, data, sizeof(uint));

    cout << "recv opcode : " << opcode << ",data: " << (char*)(data+8) << endl;

    YMessage msg = parser->getMessage(opcode);
    msg.deserialize(data, length);

    // pass message to robot controller 
    if(_listener != nullptr)
	_listener->onReceiveCommand(msg);
}

YConfigReceiver::YConfigReceiver(YConfigListener* listener, string msgformat)
{
    _listener = listener;

    parser = new YMessageFormatParser(msgformat);
    parser->parse();
}

YConfigReceiver::~YConfigReceiver()
{
    delete parser;
}

void
YConfigReceiver::onReceiveMessage(byte* data, uint length)
{
    uint opcode;
    memcpy(&opcode, data, sizeof(uint));

    YMessage msg = parser->getMessage(opcode);
    msg.deserialize(data, length);

    // pass message to robot controller 
    if(_listener != nullptr)
	_listener->onReceiveConfig(msg);
}
