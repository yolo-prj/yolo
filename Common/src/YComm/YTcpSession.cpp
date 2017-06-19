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
//	_bufferPool = new boost::pool<>(_bufferSize);	
	_valid = true;	
        _receivedSome = false;
        _continueRead = true;
}

YTcpSession::~YTcpSession(void)
{	
//	delete _bufferPool;
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
			// 접속 timeout을 설정하였으면 async_connect를 시도하고 timeout 시간만큼 timer를 설정한다.
			// 그 시간 후에 접속이 되지 않았다면 false를 리턴한다.
			// timer가 도는 도중에 접속이 되면 timer를 취소하고 true를 리턴하도록 한다.
			boost::asio::async_connect(*_tcp_socket, iterator,
				boost::bind(&YTcpSession::handle_connect, this, boost::asio::placeholders::error)
				);
			
			//boost::asio::io_service::work work(_io_service);
			boost::thread t2(boost::bind(&boost::asio::io_service::run_one, &_io_service));

			// block thread를 동작시킨다.
			_asyncWaitTimeout = true;
			boost::thread t(boost::bind(&YTcpSession::block_thread, this, timeout));
			t.join();

			// 접속 결과를 판단한다.
			connectSuccess = _asyncConnectResult;

			// timeout이 되었는데 아직 접속이 안되었다면 접속시도를 중지
			// 찰나의 순간 접속이 될 수도 있으므로 socket도 닫는다.
			if(!connectSuccess) {
				_tcp_socket->cancel();
				_tcp_socket->close();
			}
		}

		if(connectSuccess)
		{
			_tcp_socket->set_option(boost::asio::socket_base::reuse_address(true));

			_connected = true;
			// 등록된 connection callback을 호출한다.
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
		// 접속 성공
		_asyncConnectResult = true;		
	}
	else
	{
		// 접속 실패
		_asyncConnectResult = false;		
	}

	// 접속 여부가 판가름 났으면 block 스레드를 멈춘다.
	_asyncWaitTimeout = false;
}

void
YTcpSession::block_thread(unsigned int timeout)
{
	// 50ms 간격으로 check한다.
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
	_tcp_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
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
		//boost::mutex::scoped_lock lock(_mutex);

		if(_valid)
			sendLength = boost::asio::write(*_tcp_socket, boost::asio::buffer(data, dataLength));
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

	// data buffer queue에서 첫 번째 항목을 가져온다.
	std::deque<BufferInfo>::iterator itrFirst = _dataQueue.begin();
	if(itrFirst != _dataQueue.end())
	{		
		if(itrFirst->bytesTransferred == 0 || data == NULL)
		{
			// 수신받은 데이터가 없는데 queue에 들어간 것이다.
			// 이런 경우가 생기면 이는 queue에 임시로 넣어놓은 element에 데이터가 채워지기 전에
			// 읽기를 시도한 것이다. 이 경우는 그대로 return			
		}
		else if(data != NULL && itrFirst->bytesTransferred > dataLength)
		{
			memcpy(data, itrFirst->data + itrFirst->dataOffset, dataLength);
			itrFirst->dataOffset += dataLength;
			itrFirst->bytesTransferred -= dataLength;
			recvLength = dataLength;

			// 수신된 데이터가 아직 남아 있는 상황이므로 queue를 건드리지 않는다.
		}
		else if(data != NULL && itrFirst->bytesTransferred <= dataLength)
		{
			memcpy(data, itrFirst->data + itrFirst->dataOffset, itrFirst->bytesTransferred);
			recvLength = itrFirst->bytesTransferred;
			itrFirst->dataOffset = 0;
			itrFirst->bytesTransferred = 0;

			// queue에서 꺼낸 첫 번째 버퍼를 다 읽었으므로 queue에서 pop하고 data를 삭제한다.
			// queue의 다음 element는 다음 handle_read에서 처리하거나
			// 사용자 코드에서 처리가 이루어지므로 여기서 일부러 처리할 필요는 없다.
			delete[] itrFirst->data;			
			//_bufferPool->ordered_free(itrFirst->data);
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
	//readSome();	
        _readThread =  new boost::thread(boost::bind(&YTcpSession::readSomeThread, this));
}

void
YTcpSession::readSome()
{
	// 무엇인가 수신하기 전에 버퍼 정보 element를 queue에 넣어둔다.
	// 이 element 버퍼를 사용하여 데이터를 수신한다.
	BufferInfo info;
	
	// 2011-11-09 이용헌 수정
	// 버퍼를 new를 직접 사용하지 않고 boost::pool을 사용하도록 변경한다.->유보
	info.data = new unsigned char[_bufferSize];
	//ZeroMemory(info.data, _bufferSize);
	memset(info.data, 0, _bufferSize);

	_dataQueue.push_back(info);
//	std::cout << "queue count: " << _dataQueue.size() << std::endl;
	_tcp_socket->async_read_some(boost::asio::buffer(info.data, _bufferSize),
		boost::bind(&YTcpSession::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, info.data) );

//	boost::asio::io_service::work work(_io_service);
//	boost::thread t2(boost::bind(&boost::asio::io_service::run, &_io_service));			
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
            // 무엇인가 데이터를 수신하면 다음 수신 준비를 위하여 readSome()을 호출하고
            // 등록된 callback을 호출한다.
            //		printf("Some message received by async socket!\n");

            if(bytes_transferred > 0)
            {
                /*
                std::deque<BufferInfo>::iterator itrFirst = _dataQueue.begin();
                if(itrFirst != _dataQueue.end())
                {
                        // bytes_transferred 값을 설정한다.
                        itrFirst->bytesTransferred = bytes_transferred;
                }*/
                                                
                for(auto itr = _dataQueue.begin(); itr != _dataQueue.end(); ++itr)
                {
                    /*
                    if(itr->bytesTransferred == 0)
                    {
                        itr->bytesTransferred = bytes_transferred;
                        break;
                    }
                    */
                    if(itr->data == data)
                    {
                        itr->bytesTransferred = bytes_transferred;
                        break;
                    }
                }

                //readSome();

                // 수신 데이터 처리
                // 데이터 수신 이벤트를 callback한다.
                if(_receiveDataEvent != NULL)
                {
                    _receiveDataEvent->onReceiveData(*this);
                }		
            }
            else
            {
                //readSome();
            }
            
        }
        else
        {
            boost::mutex::scoped_lock lock(_mutex);
            
            _valid = false;

            // 접속이 끊어짐
            // 등록된 connection callback을 호출한다.
            if(_connectionEvent.size() != 0)
            {
                deque<YTcpConnectionEvent*>::iterator itr;
                for(itr = _connectionEvent.begin(); itr != _connectionEvent.end(); itr++) {
                    YTcpConnectionEvent* e = *itr;
                    if(e != NULL)
                        e->onDisconnected(*this);
                }
                //delete this;
                //readSome();
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

}	// namespace ycomm
}	// namespace yolo

