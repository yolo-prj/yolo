#pragma once

#include <yolo/YComm/YCommCommon.h>
#include <yolo/YComm/YTcpSession.h>

namespace yolo
{
namespace ycomm
{

class YCOMMDLL_API YTcpConnectionEvent
{
public:
	YTcpConnectionEvent(void){}
	~YTcpConnectionEvent(void){}

	virtual void onConnected(YTcpSession& session) = 0;
	virtual void onDisconnected(YTcpSession& session) = 0;
};

}	// namespace ycomm
}	// namespace yolo
