#pragma once

#include <yolo/YComm/YCommCommon.h>
#include <yolo/YComm/YTcpSession.h>

namespace yolo
{
namespace ycomm
{

class YCOMMDLL_API YTcpReceiveDataEvent
{
public:
	YTcpReceiveDataEvent(void){}
	~YTcpReceiveDataEvent(void){}

	virtual void onReceiveData(YTcpSession& session) = 0;
};

}	// namespace ycomm
}	// namespace yolo
