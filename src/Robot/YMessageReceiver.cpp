#include "YMessageReceiver.h"

YCommandReceiver::YCommandReceiver(string msgformat)
{
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

    YMessage msg = parser->getMessage(opcode);

    // pass message to robot controller 
}

YConfigReceiver::YConfigReceiver(string msgformat)
{
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

    // pass message to robot controller 
}
