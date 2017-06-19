#include <yolo/YComm/YUdpReceiveDatagramHandler.h>
#include <yolo/YComm/YUdpReceiveDatagramEvent.h>
#include <yolo/YComm/YUdp.h>
#include <algorithm>

namespace yolo
{
namespace ycomm
{

YUdpReceiveDatagramHandler::YUdpReceiveDatagramHandler()
{	
	_eventListner = NULL;	
	_bufferSize = 65535;
	_buffer = new unsigned char[_bufferSize];
	_id = 0;
	_udpObj = NULL;
}

YUdpReceiveDatagramHandler::YUdpReceiveDatagramHandler(unsigned short bufferSize)
{
	_eventListner = NULL;
	_bufferSize = bufferSize;
	_buffer = new unsigned char[_bufferSize];
	_id = 0;
	_udpObj = NULL;
}

YUdpReceiveDatagramHandler::~YUdpReceiveDatagramHandler(void)
{
	delete [] _buffer;
}

void
YUdpReceiveDatagramHandler::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
	if(!error)
	{
		// YUdp의 readSome()에서 _buffer와 _senderEndpoint를 사용하여 async_receive를 호출하고
		// udp datagram을 수신하면 이 함수가 callback 된다.
		// 여기서는 등록된 _eventListener의 onReceiveDatagram 함수를 호출해주면 된다.

		if(_eventListner != NULL && _buffer != NULL)
		{
			_eventListner->onReceiveDatagram(_senderEndpoint.address().to_string()
				, _senderEndpoint.port(), _buffer, (unsigned short)bytes_transferred);

			// 현재 handler 객체의 handle_receive가 끝났으므로 다시 UDP 객체의
			// receiveSomeAsync를 다시 호출해주어야 한다.
			// 이 때 해당 handler 객체의 handle_receive만 다시 등록하면 되므로
			// id를 인자로 넘긴다.
			if(_udpObj != NULL)
				_udpObj->receiveSomeAsync(_id);
		}
	}
	else
	{
		// 여기서는 딱히 처리해줄 것이 없는듯...
		std::cout << error.message() << std::endl;
	}
}


}	// namespace ycomm
}	// namespace yolo
