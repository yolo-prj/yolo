#pragma once

#include "YCommCommon.h"
#include <string>
#include <boost/asio.hpp>

namespace yolo
{
namespace ycomm
{

class YUdpReceiveDatagramEvent;
class YUdp;

class YUdpReceiveDatagramHandler
{
public:
	YUdpReceiveDatagramHandler(void);
	YUdpReceiveDatagramHandler(unsigned short bufferSize);
	~YUdpReceiveDatagramHandler(void);	

	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);

	inline void setID(unsigned int id) { _id = id; }
	inline void setUdpObject(YUdp* udp) { _udpObj = udp; }
	inline void setEventListener(YUdpReceiveDatagramEvent* e) { _eventListner = e; }
	inline unsigned char* getBuffer() { return _buffer; }
	inline unsigned short getBufferSize() { return _bufferSize; }

public:
	boost::asio::ip::udp::endpoint	_senderEndpoint;

private:
	unsigned int					_id;
	unsigned char*					_buffer;
	unsigned short					_bufferSize;

	YUdpReceiveDatagramEvent*		_eventListner;
	YUdp*							_udpObj;
};

}	// namespace ycomm
}	// namespace yolo
