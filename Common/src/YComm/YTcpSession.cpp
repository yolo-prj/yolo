#include <yolo/YComm/YTcpSession.h>
#include <yolo/YComm/YTcpConnectionEvent.h>
#include <yolo/YComm/YTcpReceiveDataEvent.h>
#include <boost/thread.hpp>


namespace yolo
{
namespace ycomm
{

YTcpSession::YTcpSession(boost::asio::io_service& io_service) : _io_service(io_service), _strand(io_service), _work(io_service)
{
    _tcp_socket = boost::shared_ptr<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket(io_service));
    _connected = false;
    _number = -1;	
    _receiveDataEvent = NULL;
    _asyncConnectResult = false;
    _bufferSize = defaultBufferSize;
    _valid = true;	
    _receivedSome = false;
    _continueRead = true;
}

YTcpSession::~YTcpSession(void)
{	
    _continueRead = false;
    _readThread->join();
    delete _readThread;

    std::deque<BufferInfo>::iterator itr;
    for(itr = _dataQueue.begin(); itr != _dataQueue.end(); itr++)
    {
	unsigned char* data = itr->data;
	if(data != NULL)
	    delete[] data;
    }
}


bool
YTcpSession::connect(std::string serverAddress, unsigned short serverPort, unsigned int timeout)
{
    char buffer[10];
    sprintf(buffer, "%d", serverPort);
    bool connectSuccess = false;

    try
    {
	boost::asio::ip::tcp::resolver resolver(_io_service);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), serverAddress, buffer);
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
	
	if(timeout == 0)
	{
	    boost::asio::connect(*_tcp_socket, iterator);
	    
	    connectSuccess = true;						
	}
	else
	{
	    boost::asio::async_connect(*_tcp_socket, iterator,
		boost::bind(&YTcpSession::handle_connect, this, boost::asio::placeholders::error)
		);
	    
	    _asyncWaitTimeout = true;
	    boost::thread t(boost::bind(&YTcpSession::block_thread, this, timeout));
	    t.join();

	    connectSuccess = _asyncConnectResult;

	    if(!connectSuccess) {
		_tcp_socket->cancel();
		_tcp_socket->close();
	    }
	}

	if(connectSuccess)
	{
	    _tcp_socket->set_option(boost::asio::socket_base::reuse_address(true));

	    _connected = true;
	    if(_connectionEvent.size() != 0)
	    {
		deque<YTcpConnectionEvent*>::iterator itr;
		for(itr = _connectionEvent.begin(); itr != _connectionEvent.end(); itr++) {
			YTcpConnectionEvent* e = *itr;
			if(e != NULL)
				e->onConnected(*this);
		}
	    }
	}
	
    }
    catch(std::exception& e)
    {
	_connected = false;
	std::cerr << "[YComm] Exception: " << e.what() << std::endl;		
    }

    return connectSuccess;
}

void
YTcpSession::handle_connect(const boost::system::error_code& ec)
{
    if(!ec)
    {
	_asyncConnectResult = true;		
    }
    else
    {
	_asyncConnectResult = false;		
    }

    _asyncWaitTimeout = false;
}

void
YTcpSession::block_thread(unsigned int timeout)
{
	const int TIME_STEP = 10;	// 10ms

	while(_asyncWaitTimeout && timeout > 0) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(TIME_STEP));
		timeout -= TIME_STEP;
	}
}

bool
YTcpSession::disconnect()
{
	boost::system::error_code error;
	try{
	    _tcp_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	} catch(...){}
	
	_tcp_socket->close(error);

	if(!error)
		return true;
	else
		return false;
}

std::string
YTcpSession::getPeerAddress()
{
	std::string addr;

	if(_tcp_socket->is_open())
	{
		addr = _tcp_socket->remote_endpoint().address().to_string();
	}
	else
	{
		addr = "";
	}

	return addr;
}

unsigned short
YTcpSession::getPeerPort()
{	
	unsigned short port = 0;

	if(_tcp_socket->is_open())
	{
		port = _tcp_socket->remote_endpoint().port();
	}

	return port;
}

std::string
YTcpSession::getLocalAddress()
{
	std::string addr;

	if(_tcp_socket->is_open())
	{
		addr = _tcp_socket->local_endpoint().address().to_string();		
	}
	else
	{
		addr = "";
	}

	return addr;
}

unsigned short
YTcpSession::getLocalPort()
{
	unsigned short port = 0;

	if(_tcp_socket->is_open())
	{
		port = _tcp_socket->local_endpoint().port();
	}

	return port;
}

void
YTcpSession::registerConnectionEventCallback(YTcpConnectionEvent* e)
{
	_connectionEvent.push_back(e);
}

void
YTcpSession::registerReceiveDataEventCallback(YTcpReceiveDataEvent* e)
{
	_receiveDataEvent = e;
}

size_t
YTcpSession::send(unsigned char* data, size_t dataLength)
{
	size_t sendLength = 0;

	try {
//		boost::mutex::scoped_lock lock(_mutex);

		if(_valid)
			//sendLength = boost::asio::write(*_tcp_socket, boost::asio::buffer(data, dataLength));
			boost::asio::async_write(*_tcp_socket, boost::asio::buffer(data, dataLength)
				, boost::bind(&YTcpSession::sendHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	catch(boost::system::system_error& e) {		
		std::cerr << "[YComm] TCP send failed" << std::endl;
		std::cerr << "[YComm] Exception: " << e.what() << std::endl;
	}

	return sendLength;
}

size_t
YTcpSession::recv(unsigned char* data, size_t dataLength)
{	
	size_t recvLength = 0;	

	std::deque<BufferInfo>::iterator itrFirst = _dataQueue.begin();
	if(itrFirst != _dataQueue.end())
	{		
		if(itrFirst->bytesTransferred == 0 || data == NULL)
		{
		}
		else if(data != NULL && itrFirst->bytesTransferred > dataLength)
		{
			memcpy(data, itrFirst->data + itrFirst->dataOffset, dataLength);
			itrFirst->dataOffset += dataLength;
			itrFirst->bytesTransferred -= dataLength;
			recvLength = dataLength;
		}
		else if(data != NULL && itrFirst->bytesTransferred <= dataLength)
		{
			memcpy(data, itrFirst->data + itrFirst->dataOffset, itrFirst->bytesTransferred);
			recvLength = itrFirst->bytesTransferred;
			itrFirst->dataOffset = 0;
			itrFirst->bytesTransferred = 0;

			delete[] itrFirst->data;			
			_dataQueue.pop_front();
		}
	}	
	
	return recvLength;
}


void
YTcpSession::start()
{	
	_tcp_socket->set_option(boost::asio::socket_base::reuse_address(true));
        _receivedSome = true;
        _readThread =  new boost::thread(boost::bind(&YTcpSession::readSomeThread, this));
}

void
YTcpSession::readSome()
{
	BufferInfo info;
	
	info.data = new unsigned char[_bufferSize];
	memset(info.data, 0, _bufferSize);

	{
	    boost::mutex::scoped_lock lock(_mutex);
	    _dataQueue.push_back(info);
	    _tcp_socket->async_read_some(boost::asio::buffer(info.data, _bufferSize),
		    boost::bind(&YTcpSession::handle_read, this,
			    boost::asio::placeholders::error,
			    boost::asio::placeholders::bytes_transferred, info.data) );
	}
}

void
YTcpSession::handle_read(const boost::system::error_code& error, size_t bytes_transferred, unsigned char* data)
{
    if(!_valid)
    {		
        delete this;
    }
    else
    {
        _receivedSome = true;
        if(!error)
        {
            if(bytes_transferred > 0)
            {
		boost::mutex::scoped_lock lock(_mutex);
                for(auto itr = _dataQueue.begin(); itr != _dataQueue.end(); ++itr)
                {
                    if(itr->data == data)
                    {
                        itr->bytesTransferred = bytes_transferred;
                        break;
                    }
                }

                if(_receiveDataEvent != NULL)
                {
                    _receiveDataEvent->onReceiveData(*this);
//		    boost::thread t(boost::bind(&YTcpSession::receiveDataCallback, this));
                }		
            }
            else
            {
            }
            
        }
        else
        {
//            boost::mutex::scoped_lock lock(_mutex);
            
            _valid = false;

            if(_connectionEvent.size() != 0)
            {
                deque<YTcpConnectionEvent*>::iterator itr;
                for(itr = _connectionEvent.begin(); itr != _connectionEvent.end(); itr++) {
                    YTcpConnectionEvent* e = *itr;
                    if(e != NULL)
                        e->onDisconnected(*this);
                }
                _receivedSome = true;
            }
        }
    }	
}

void
YTcpSession::handle_send(const boost::system::error_code& error, size_t bytes_transferred)
{
	if(!_valid)
	{		
	}
	else
	{
		if(!error)
		{	
			
		}
		else
		{
			cerr << "[YTcpSession::handle_send] send failed!!!! : " << bytes_transferred << endl;
		}
	}	
}

int
YTcpSession::getSocketHandle()
{
	boost::asio::ip::tcp::socket::native_handle_type handle = _tcp_socket->native_handle();
	
	return (int)handle;
}

void
YTcpSession::readSomeThread()
{
    boost::thread t(boost::bind(&boost::asio::io_service::run, &_io_service));

    while(_continueRead)
    {
        if(_receivedSome) {
            _receivedSome = false;
            readSome();
        } else {
            boost::this_thread::sleep(boost::posix_time::milliseconds(5));
        }
    }
}

void
YTcpSession::sendHandler(const boost::system::error_code& error, size_t bytes_transferred)
{
}

void
YTcpSession::receiveDataCallback()
{
    _receiveDataEvent->onReceiveData(*this);
}

}	// namespace ycomm
}	// namespace yolo

