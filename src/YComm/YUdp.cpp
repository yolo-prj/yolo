#include <yolo/YComm/YComm.h>
#include <yolo/YComm/YUdp.h>
#include <yolo/YComm/YUdpReceiveDatagramEvent.h>
#include <yolo/YComm/YUdpReceiveDatagramHandler.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <algorithm>

namespace yolo
{
namespace ycomm
{

YUdp::YUdp(std::string localAddress, unsigned short localport , std::string remoteAddress, unsigned short remoteport, CastType castType)
	: _LAddress(localAddress), _LPort(localport), _RAddress(remoteAddress), _RPort(remoteport), _CastType(castType),  _valid(false), _receiveEvent(NULL)
{
	_udp_socket = NULL;
	_receivedDatagram = NULL;
	_receivedDatagramForHandler = NULL;
	_multicastPort = 0;
	_continueReceive = true;
	_io_service = &YComm::getInstance().getIOService();
	_strand = new boost::asio::strand(*_io_service);
	_work = new boost::asio::io_service::work(*_io_service);
}

YUdp::YUdp(std::string localAddress, unsigned short localport, CastType castType)
	: _LAddress(localAddress), _LPort(localport), _RAddress(""), _RPort(0), _CastType(castType), _valid(false), _receiveEvent(NULL)
{
	_udp_socket = NULL;
	_receivedDatagram = NULL;
	_receivedDatagramForHandler = NULL;
	_multicastPort = 0;
	_continueReceive = true;
	_io_service = &YComm::getInstance().getIOService();
	_strand = new boost::asio::strand(*_io_service);
	_work = new boost::asio::io_service::work(*_io_service);
}

YUdp::YUdp(unsigned short port, CastType castType)
	: _LPort(port), _RPort(0), _CastType(castType), _valid(false), _receiveEvent(NULL)
{
	_udp_socket = NULL;
	_receivedDatagram = NULL;
	_receivedDatagramForHandler = NULL;
	_multicastPort = 0;
	_continueReceive = true;
	_io_service = &YComm::getInstance().getIOService();
	_strand = new boost::asio::strand(*_io_service);
	_work = new boost::asio::io_service::work(*_io_service);
}

YUdp::~YUdp(void)
{
	_continueReceive = false;
	_receiveThread->join();
	delete _receiveThread;

	close();

	delete _work;
	delete _strand;

	if(_receivedDatagram != NULL)
		delete[] _receivedDatagram;

	if(_receivedDatagramForHandler != NULL)
		delete[] _receivedDatagramForHandler;

			BOOST_FOREACH(YUdpReceiveDatagramHandler* handler, _handlerList)
			{
				delete handler;
			}
			//std::for_each(_handlerList.begin(), _handlerList.end(), [](YUdpReceiveDatagramHandler* handler){
			//	delete handler;
			//});
}

bool
YUdp::open()
{
	bool success = false;

	boost::system::error_code error;

	if(_udp_socket == NULL)
	{
		try {

			// UDP socket 통신을 위한 local endpoint 생성 및 socket open
			boost::asio::io_service& io = YComm::getInstance().getIOService();
			if(_LAddress.empty())
			{
				_udp_socket = new boost::asio::ip::udp::socket(io
					, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), _LPort) );
			}
			else
			{
				if(_CastType == MULTICAST)
				{
					_udp_socket = new boost::asio::ip::udp::socket(io);						
				}
				else
				{
					_udp_socket = new boost::asio::ip::udp::socket(io
						, boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(_LAddress), _LPort) );
				}				

				_udp_socket->set_option(boost::asio::socket_base::reuse_address(true));
			}
			success = true;
			_valid = true;

			if(_CastType == UNICAST)
			{
				boost::system::error_code error;
				boost::asio::ip::udp::resolver resolver(io);
				
				string addr;
				unsigned short port;

				if(!_RAddress.empty())
				{
					addr = _RAddress;
					port = _RPort;
				}
				else
				{
					addr = _LAddress;
					port = _LPort;
				}

				char bufferSender[10];
				sprintf(bufferSender, "%d", port);
				
				boost::asio::ip::udp::resolver::query querySender(boost::asio::ip::udp::v4(), addr, bufferSender);
				boost::asio::ip::udp::resolver::query querySenderLocal(boost::asio::ip::udp::v4(), bufferSender);
				boost::asio::ip::udp::resolver::iterator iteratorSender;

				if(!addr.empty())
					iteratorSender = resolver.resolve(querySender, error);
				else
					iteratorSender = resolver.resolve(querySenderLocal, error);

				if(!error)
				{
					_senderEndpoint = *iteratorSender;

					// 2013-02-18 이용헌 추가
					// 설정된 senderEndpoint에 대해 async_recv_from을 호출한다.
					//receiveSome();						
				}					
				else
				{

				}

				char bufferReceiver[10];
				sprintf(bufferReceiver, "%d", _LPort);
				boost::asio::ip::udp::resolver::query queryReceiver(boost::asio::ip::udp::v4(), _LAddress, bufferReceiver);
				boost::asio::ip::udp::resolver::iterator iteratorReceiver = resolver.resolve(queryReceiver, error);

				if(!error)
				{
					_receiverEndpoint = *iteratorReceiver;							
				}	
				else
				{

				}

				// asynchronous receivefrom 루틴을 실행한다.
				//receiveSomeAsync();
			}
			else if(_CastType == BROADCAST)
			{
				boost::system::error_code error;
				boost::asio::ip::udp::resolver resolver(io);

				_udp_socket->set_option(boost::asio::ip::udp::socket::reuse_address(true));
				_udp_socket->set_option(boost::asio::socket_base::broadcast(true));
				
				if(_RPort != 0)
				{
					boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), _RPort); 
					_senderEndpoint = endpoint;
				}
				else
				{

				}

				char bufferReceiver[10];
				sprintf(bufferReceiver, "%d", _LPort);
				boost::asio::ip::udp::resolver::query queryReceiver(boost::asio::ip::udp::v4(), _LAddress, bufferReceiver);
				boost::asio::ip::udp::resolver::iterator iteratorReceiver = resolver.resolve(queryReceiver, error);

						if(!error)
						{
							_receiverEndpoint = *iteratorReceiver;							
						}						
					}
					else if(_CastType == MULTICAST)
					{	
						if(_RPort != 0)
						{
							boost::asio::ip::udp::endpoint endpoint(_multicastAddress, _multicastPort); 
							_senderEndpoint = endpoint;
						}

				boost::system::error_code error;
				boost::asio::ip::udp::resolver resolver(io);

				char bufferReceiver[10];
				sprintf(bufferReceiver, "%d", _multicastPort);
				boost::asio::ip::udp::resolver::query queryReceiver(boost::asio::ip::udp::v4(), _LAddress, bufferReceiver);
				boost::asio::ip::udp::resolver::iterator iteratorReceiver = resolver.resolve(queryReceiver, error);

				if(!error)
				{
					_receiverEndpoint = *iteratorReceiver;							

					_udp_socket->open(_receiverEndpoint.protocol());
					_udp_socket->set_option(boost::asio::ip::udp::socket::reuse_address(true));
					_udp_socket->bind(_receiverEndpoint);

					_udp_socket->set_option(boost::asio::ip::multicast::join_group(_multicastAddress));

					//receiveSome();
				}
			}

		}
		catch(boost::system::system_error& e) {
			std::cerr << "[YUdp] " << e.what() << std::endl;
			success = false;
			_valid = false;
		}

		if(success) {
		    _receivedSome = true;
		    _receiveThread = new boost::thread(boost::bind(&YUdp::receiveSomeThread, this) );
		}
	}

	return success;
}

void
YUdp::close()
{
	if(_udp_socket != NULL)
	{
		_udp_socket->close();
		delete _udp_socket;
		_udp_socket = NULL;
		_valid = false;				
	}
}

size_t
YUdp::sendTo(unsigned char* data, size_t dataLength)
{
	size_t sendLength = 0;

	if(_udp_socket != NULL && _udp_socket->is_open()	&& _valid)
	{
		try {
			sendLength = _udp_socket->send_to(boost::asio::buffer(data, dataLength), _senderEndpoint);
		} catch(boost::system::system_error& e) {
			std::cerr << "[YUdp] UDP datagram send failed" << std::endl;
			std::cerr << "[YUdp] Exception: " << e.what() << std::endl;
		}
	}
	return sendLength;
}

size_t
YUdp::sendTo(unsigned char* data, size_t dataLength, std::string remoteAddress, unsigned short remoteport)
{
	size_t sendLength = 0;

	boost::asio::io_service& io = YComm::getInstance().getIOService();

	if(_udp_socket != NULL && _udp_socket->is_open()	&& _valid)
	{
		// remote address, port에 대한 endpoint를 만든다.
		boost::system::error_code error;
				boost::asio::ip::udp::resolver resolver(io);
		boost::asio::ip::udp::endpoint remoteEndpoint;

		try {

			char bufferSender[10];
			sprintf(bufferSender, "%d", remoteport);
			boost::asio::ip::udp::resolver::query querySender(boost::asio::ip::udp::v4(), remoteAddress, bufferSender);
			boost::asio::ip::udp::resolver::iterator iteratorSender = resolver.resolve(querySender, error);

			if(!error)
			{
				remoteEndpoint = *iteratorSender;							
				sendLength = _udp_socket->send_to(boost::asio::buffer(data, dataLength), remoteEndpoint);
			}					
			else
			{

			}					
					
		} catch(boost::system::system_error& e) {
			std::cerr << "[YUdp] UDP datagram send failed" << std::endl;
			std::cerr << "[YUdp] Exception: " << e.what() << std::endl;
		}
	}
	return sendLength;
}

size_t
YUdp::recvFrom(unsigned char* data, size_t dataLength)
{
	// recvFrom 호출시 주어지는 data 버퍼는 YUdp 외부의 버퍼로써
	// 미리 메모리 할당되어 있어야 한다.
	size_t recvLength = 0;

	if(_udp_socket != NULL && _udp_socket->is_open() && _valid)
	{
		try {
			recvLength = _udp_socket->receive_from(boost::asio::buffer(data, dataLength), _receiverEndpoint);			
		} catch(boost::system::system_error& e) {
			std::cerr << "[YUdp] UDP datagram recv failed" << std::endl;
			std::cerr << "[YUdp] Exception: " << e.what() << std::endl;
		}
	}
	return recvLength;
}

size_t
YUdp::recvFrom(unsigned char* data, size_t dataLength, std::string remoteAddress, unsigned short remoteport)
{
	// recvFrom 호출시 주어지는 data 버퍼는 YUdp 외부의 버퍼로써
	// 미리 메모리 할당되어 있어야 한다.
	size_t recvLength = 0; 

	boost::asio::io_service& io = YComm::getInstance().getIOService();

	if(_udp_socket != NULL && _udp_socket->is_open() && _valid)
	{
		boost::system::error_code error;
		boost::asio::ip::udp::resolver resolver(io);
		boost::asio::ip::udp::endpoint remoteEndpoint;

		try {

			char bufferSender[10];
			sprintf(bufferSender, "%d", remoteport);
			boost::asio::ip::udp::resolver::query querySender(boost::asio::ip::udp::v4(), remoteAddress, bufferSender);
			boost::asio::ip::udp::resolver::iterator iteratorSender = resolver.resolve(querySender, error);

			if(!error)
			{
				remoteEndpoint = *iteratorSender;							
				recvLength = _udp_socket->receive_from(boost::asio::buffer(data, dataLength), remoteEndpoint);
			}					
			else
			{

			}
					
		} catch(boost::system::system_error& e) {
			std::cerr << "[YUdp] UDP datagram recv failed" << std::endl;
			std::cerr << "[YUdp] Exception: " << e.what() << std::endl;
		}
	}
	return recvLength;
}

void
YUdp::addReceiveDatagramEvent(YUdpReceiveDatagramEvent* e, std::string remoteAddress, unsigned short remoteport)
{
	boost::asio::io_service& io = YComm::getInstance().getIOService();

	boost::system::error_code error;
	boost::asio::ip::udp::resolver resolver(io);
	boost::asio::ip::udp::endpoint remoteEndpoint;

	try {
		char bufferSender[10];
		sprintf(bufferSender, "%d", remoteport);
		boost::asio::ip::udp::resolver::query querySender(boost::asio::ip::udp::v4(), remoteAddress, bufferSender);
		boost::asio::ip::udp::resolver::iterator iteratorSender = resolver.resolve(querySender, error);

		if(!error)
		{
			remoteEndpoint = *iteratorSender;
		
			// YUdpReceiveDatagramHandler 객체를 생성하여 list에 추가한다.
			// 이 객체는 중간에 list에서 삭제되지 않는다.
			// 즉, 동적으로 특정 remote ip/port에서 UDP datagram 수신 여부를 정하지 못한다는 것이다.
			// (vector에 저장된 handler 객체를 삭제하고 NULL로 만드는 방법이 있긴 하지만 따로 요구사항이 발생하지 않으면 구현하지 않는다.)
			YUdpReceiveDatagramHandler* handler = new YUdpReceiveDatagramHandler();			
			handler->_senderEndpoint = remoteEndpoint;
			handler->setID(_handlerList.size());
			handler->setUdpObject(this);
			handler->setEventListener(e);

			_handlerList.push_back(handler);
		}					
		else
		{
		}

	} catch(boost::system::system_error& e) {
		std::cerr << "[YUdp] UDP datagram receive handler registration failed" << std::endl;
		std::cerr << "[YUdp] Exception: " << e.what() << std::endl;
	}
}

void
YUdp::receiveSome()
{
	if(_receiveEvent)
	{		
		try {
			if(_receivedDatagram == NULL) {
				_receivedDatagram = new unsigned char[_receiveEvent->maxDatagramLength];
				memset(_receivedDatagram, 0, _receiveEvent->maxDatagramLength);
			}			

			if(_receivedDatagramForHandler == NULL)
			{
				_receivedDatagramForHandler = new unsigned char[_receiveEvent->maxDatagramLength];
				memset(_receivedDatagramForHandler, 0, _receiveEvent->maxDatagramLength);
			}

			_udp_socket->async_receive_from(boost::asio::buffer(_receivedDatagram, _receiveEvent->maxDatagramLength)
				, _senderEndpoint, boost::bind(&YUdp::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );			

		} catch(boost::system::system_error& e) {
			std::cerr << "[YUdp] UDP datagram async recvFrom failed" << std::endl;
			std::cerr << "[YUdp] Exception: " << e.what() << std::endl;
		}
	}
	else
	{
	}
}

void
YUdp::receiveSomeAsync()
{	
	// handler list의 listner 객체의 handler도 등록한다.
	try {
		//std::for_each(_handlerList.begin(), _handlerList.end(), [this](YUdpReceiveDatagramHandler* handler) {
		//	_udp_socket->async_receive_from(boost::asio::buffer(handler->getBuffer(), handler->getBufferSize())
		//		, handler->_senderEndpoint, boost::bind(&YUdpReceiveDatagramHandler::handle_receive, handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
		//});


		BOOST_FOREACH(YUdpReceiveDatagramHandler* handler, _handlerList)
		{
			_udp_socket->async_receive_from(boost::asio::buffer(handler->getBuffer(), handler->getBufferSize())
				, handler->_senderEndpoint, boost::bind(&YUdpReceiveDatagramHandler::handle_receive, handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
		}
	}
	catch(boost::system::system_error& e) {
		std::cerr << "[YUdp] UDP datagram async recvFrom failed" << std::endl;
		std::cerr << "[YUdp] Exception: " << e.what() << std::endl;
	}	
}

void
YUdp::receiveSomeAsync(unsigned int i)
{
	// 등록된 listener 객체 중에서 지정된 하나의 handler만 async_receive를 수행한다.
	try {
		YUdpReceiveDatagramHandler* handler = _handlerList[i];

		if(handler != NULL)
		{
			_udp_socket->async_receive_from(boost::asio::buffer(handler->getBuffer(), handler->getBufferSize())
				, handler->_senderEndpoint, boost::bind(&YUdpReceiveDatagramHandler::handle_receive, handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );

		}		
	}
	catch(boost::system::system_error& e) {
		std::cerr << "[YUdp] UDP datagram async recvFrom failed" << std::endl;
		std::cerr << "[YUdp] Exception: " << e.what() << std::endl;
	}
}

void
YUdp::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
//	boost::mutex::scoped_lock lock(_mutex);

	if(!error)
	{
		// 무엇인가 성공적으로 datagram을 수신했음
		_receivedDatagramLength = (unsigned short)bytes_transferred;

		// 2013-12-18 이용헌 수정
		// 수신된 datagram을 buffer로 복사하여 사용
		memcpy(_receivedDatagramForHandler, _receivedDatagram, _receivedDatagramLength);

		//receiveSome();
		_receivedSome = true;
		
		// 수신 데이터 처리
		if(_receiveEvent != NULL)
			_receiveEvent->onReceiveDatagram(_senderEndpoint.address().to_string(), _senderEndpoint.port()
			, _receivedDatagramForHandler, _receivedDatagramLength);
	}
	else
	{
		// UDP의 경우 이쪽으로 어떤 경우에 들어오는지 확인 필요
		// socket을 닫았을 때?
		std::cerr << "[YUdp] async_receive failed : " << error.message() << std::endl;
	}
}

void
YUdp::receiveSomeThread()
{
    boost::thread t(boost::bind(&boost::asio::io_service::run, _io_service));

    while(_continueReceive)
    {
	if(_receivedSome) {
	    _receivedSome = false;
	    receiveSome();
	} else {
	    boost::this_thread::sleep(boost::posix_time::milliseconds(5));
	}
    }
}

}	// namespace ycomm
}	// namespace yolo
