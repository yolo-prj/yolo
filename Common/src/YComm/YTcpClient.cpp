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
    }
}

void
YTcpClient::onConnected(YTcpSession& session)
{
}

void
YTcpClient::onDisconnected(YTcpSession& session)
{
    if(_session == &session)
    {
	if(_isTryToReconnect && !_disconnecting)
	{
	    boost::asio::io_service& io = YComm::getInstance().getIOService();
	    _session = new YTcpSession(io);
	    _session->setBufferSize(_bufferSize);

	    _session->registerConnectionEventCallback(_c_event);			
	    _session->registerConnectionEventCallback(this);
	    _session->registerConnectionEventCallback(&YComm::getInstance());
	    
	    _session->registerReceiveDataEventCallback(_r_event);

	    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	    auto bf = boost::bind(&YTcpClient::reconnectThread, this, _1);

	    boost::thread t(bf, (void*)_session);
	}
    }
}

bool
YTcpClient::connect(unsigned int timeout)
{
    bool success = false;

    boost::asio::io_service& io = YComm::getInstance().getIOService();
    _session = new YTcpSession(io);
    _session->setBufferSize(_bufferSize);

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
	auto bf = boost::bind(&YTcpClient::reconnectThread, this, _1);

	boost::thread t(bf, (void*)_session);

	success = true;

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
	//success = session->connect(_serverAddress, _serverPort, 1000);
	success = session->connect(_serverAddress, _serverPort);

	if(!success)
	{
	    cout << "[YTcpClient] reconnect failed... trying to reconnect...(" << _serverAddress << ":" << _serverPort << ")" << endl;
	}
	else
	{
	    session->start();
	    break;
	}

	boost::this_thread::sleep(boost::posix_time::seconds(2));
    } while(!success);
}

}	// namespace ycomm
}	// namespace yolo

