#pragma once

#include <string>
#include "YCommCommon.h"

namespace yolo
{
namespace ycomm
{

class YCOMMDLL_API YUdpReceiveDatagramEvent
{
public:
	YUdpReceiveDatagramEvent(void){ maxDatagramLength = 65535; }
	~YUdpReceiveDatagramEvent(void){}

	virtual void onReceiveDatagram(std::string remoteAddress, unsigned short remotePort, unsigned char* data, unsigned short length) = 0;

public:
	std::string		remoteAddress;
	unsigned short	remotePort;
	unsigned short	maxDatagramLength;
};

}	// namespace ycomm
}	// namespace yolo
