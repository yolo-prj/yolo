#include "YMessageSender.h"

YImageSender::YImageSender(YNetworkManager* manager)
{
    this->manager = manager;
}

YImageSender::~YImageSender()
{
}

void
YImageSender::send(YMessage& msg)
{
    uint length = 0;
    byte* data = msg.serialize(length);

    // image will be sent via UDP
    manager->sendUdpDatagram(msg.getOpcode(), data, length);
}

YEventSender::YEventSender(YNetworkManager* manager)
{
    this->manager = manager;
}

YEventSender::~YEventSender()
{
}

void
YEventSender::send(YMessage& msg)
{
    uint length = 0;
    byte* data = msg.serialize(length);

    boost::shared_ptr<byte[]> ptr = boost::make_shared<byte[]>(length);
    memcpy(ptr.get(), data, length);

    //manager->sendTcpPacket(0, msg.getOpcode(), ptr, length);
    manager->sendTcpPacket(0, msg.getOpcode(), data, length);

    cout << "opcode: " << msg.getOpcode() << " data : " << (char*)(data + 8) << endl;
}

