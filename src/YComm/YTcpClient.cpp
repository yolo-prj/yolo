#include <yolo/YComm/YTcpClient.h>
#include <yolo/YComm/YTcpSession.h>
#include <yolo/YComm/YComm.h>
#include <boost/thread.hpp>

namespace yolo
{
namespace ycomm
{

YTcpClient::YTcpClient(std::string serverAddress, unsigned short serverPort) 
	: _serverAddress(serverAddress), _serverPort(serverPort)
{
	_c_event = NULL;
	_r_event = NULL;

	_session = NULL;

	_connected = false;

	_bufferSize = defaultBufferSize;

	_isTryToReconnect = false;
	_disconnecting = false;
}

YTcpClient::YTcpClient(std::string serverAddress, unsigned short serverPort, bool isTryToReconnect)
	: _serverAddress(serverAddress), _serverPort(serverPort)
{
	_c_event = NULL;
	_r_event = NULL;

	_session = NULL;

	_connected = false;

	_bufferSize = defaultBufferSize;

	_isTryToReconnect = isTryToReconnect;
	_disconnecting = false;

}

YTcpClient::~YTcpClient(void)
{
	if(_session != NULL) {
		//_session->disconnect();
		//delete _session;
	}
}

void
YTcpClient::onConnected(YTcpSession& session)
{
	// 접속이 되었으면 지금 딱히 할 것은 없다.
}

void
YTcpClient::onDisconnected(YTcpSession& session)
{
	if(_session == &session)
	{
		//_session = NULL;
		// 실제 session 객체는 session에서 알아서 지우므로 여기서는 처리할 필요가 없다.
		// TcpClient에서 session을 생성하기 때문에 이 클래스에서 지우는 것이 원칙이지만
		// onDisconnected는 callback이며 이 callback은 다른 클래스도 받을 수 있기 때문에
		// 모든 callback이 처리된 이후에 session 클래스에서 자신을 지우도록 하는 것이 맞다.


		if(_isTryToReconnect && !_disconnecting)
		{
			// 재접속 flag가 활성화되어 있으면 접속을 다시 시도한다.

			// TCP session을 생성하여 접속을 시도한다.
			boost::asio::io_service& io = YComm::getInstance().getIOService();
			_session = new YTcpSession(io);
			_session->setBufferSize(_bufferSize);
			// TCP session 객체에 connection, recv data 이벤트 콜백을 등록한다.			
			_session->registerConnectionEventCallback(_c_event);			
			_session->registerConnectionEventCallback(this);
			_session->registerConnectionEventCallback(&YComm::getInstance());
			
			_session->registerReceiveDataEventCallback(_r_event);

			auto bf = boost::bind(&YTcpClient::reconnectThread, this, _1);
			//_threadPool->runThread(bf, (void*)_session);
			boost::thread t(bf, (void*)_session);
		}

	}
}

bool
YTcpClient::connect(unsigned int timeout)
{
	bool success = false;

	//if(_io_service != NULL)
	{		
		// TCP session을 생성하여 접속을 시도한다.
		boost::asio::io_service& io = YComm::getInstance().getIOService();
		_session = new YTcpSession(io);
		_session->setBufferSize(_bufferSize);
		// TCP session 객체에 connection, recv data 이벤트 콜백을 등록한다.		
		_session->registerConnectionEventCallback(_c_event);
		_session->registerConnectionEventCallback(this);
		_session->registerConnectionEventCallback(&YComm::getInstance());
		
		_session->registerReceiveDataEventCallback(_r_event);
		
		if(!_isTryToReconnect)
		{
			success = _session->connect(_serverAddress, _serverPort, timeout);
			
			if(success)
				_session->start();		
		}
		else
		{
			// 재접속 thread 실행
			auto bf = boost::bind(&YTcpClient::reconnectThread, this, _1);
			//_threadPool->runThread(bf, (void*)_session);
			boost::thread t(bf, (void*)_session);

			success = true;

			cout << "[YTcpClient] connection failed: trying to reconnect....(" << _serverAddress << ":" << _serverPort << ")" << endl;
		}		
	}

	return success;
}

bool
YTcpClient::disconnect()
{
	bool success = false;

	if(_session != NULL)
	{
		_disconnecting = true;
		success = _session->disconnect();
		//delete _session;
		//_session = NULL;
	}

	return success;
}

size_t
YTcpClient::send(unsigned char* data, size_t dataLength)
{
	if(_session != NULL && _session->getSocket().is_open())
	{
		return _session->send(data, dataLength);
	}
	else
		return 0;
}

size_t
YTcpClient::recv(unsigned char* data, size_t dataLength)
{
	if(_session != NULL && _session->getSocket().is_open())
	{
		return _session->recv(data, dataLength);
	}
	else
		return 0;
}


void
YTcpClient::reconnectThread(void* arg)
{
	YTcpSession* session = (YTcpSession*)arg;

	bool success = false;
	do
	{
		success = session->connect(_serverAddress, _serverPort, 1000);

		if(!success)
		{
			cout << "[YTcpClient] reconnect failed... trying to reconnect...(" << _serverAddress << ":" << _serverPort << ")" << endl;
		}
		else
		{
			session->start();
			break;
		}

		boost::this_thread::sleep(boost::posix_time::seconds(1));
	} while(!success);
}

}	// namespace ycomm
}	// namespace yolo

