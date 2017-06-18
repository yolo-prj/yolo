#include <yolo/YComm/YTcpServer.h>
#include <yolo/YComm/YComm.h>
#include <boost/foreach.hpp>


namespace yolo
{
namespace ycomm
{

YTcpServer::YTcpServer(std::string address, unsigned short port)
	: _address(address), _port(port)
{
	_c_event = NULL;
	_r_event = NULL;
	_acceptor = NULL;
	_startServer = false;
	_bufferSize = defaultBufferSize;
}

YTcpServer::YTcpServer(unsigned short port)
	: _port(port)
{
	_address = "";
	_c_event = NULL;
	_r_event = NULL;
	_acceptor = NULL;
	_startServer = false;
	_bufferSize = defaultBufferSize;
}

YTcpServer::~YTcpServer(void)
{
	stopServer();
}

void
YTcpServer::onConnected(YTcpSession& session)
{
	// 접속이 되었으면 지금 딱히 할 것은 없다.
	// accept시에 session이 새로 생성되어 set으로 관리되기 때문이다.
}

void
YTcpServer::onDisconnected(YTcpSession& session)
{
	
	std::set<YTcpSession*>::iterator itr;
	itr = _setSession.find(&session);
	if(itr != _setSession.end())
		_setSession.erase(itr);
	
	// 실제 session 객체는 session에서 알아서 지우므로 여기서는 삭제 처리할 필요가 없다.	
}

bool
YTcpServer::accept()
{
	bool success = false;

	//if(_io_service != NULL)
	{
		if(_acceptor == NULL) {
			boost::asio::io_service& io = YComm::getInstance().getIOService();
			_acceptor = new boost::asio::ip::tcp::acceptor(io);
		}
		
		try {
			boost::system::error_code error;

			// acceptor를 사용하여 socket open-bind-listen을 차례로 수행한다.
			// 한번 listen까지 수행하면 이후에 accept만 수행하면 된다.
			if(!_acceptor->is_open())
			{
				_acceptor->open(boost::asio::ip::tcp::v4() );

				if(_address.empty()) {
					_acceptor->bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _port), error);
				}
				else {
					_acceptor->bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(_address), _port), error);
				}

				if(!error)
					_acceptor->listen();
			}			

			if(!error)
			{
				boost::asio::io_service& io = YComm::getInstance().getIOService();
                                boost::asio::strand _strand(io);
				YTcpSession* new_session = new YTcpSession(io);
				new_session->setBufferSize(_bufferSize);
				// TCP session 객체에 connection, recv data 이벤트 콜백을 등록한다.
				new_session->registerConnectionEventCallback(this);
				new_session->registerConnectionEventCallback(_c_event);
				new_session->registerReceiveDataEventCallback(_r_event);

				// accept 시작!
				_acceptor->async_accept(new_session->getSocket(),
					boost::bind(&YTcpServer::handle_accept, this, new_session,
					boost::asio::placeholders::error));

				//boost::asio::io_service::work work(io);
				boost::thread t(boost::bind(&boost::asio::io_service::run_one, &io));

				success = true;				
			}
			else
			{
				std::cerr << "[YComm] TCP server socket bind failed: " << _address << " " << _port << std::endl;
				_acceptor->cancel();
				_acceptor->close();
				delete _acceptor;
				_acceptor = NULL;
				success = false;
			}			
		} catch(std::exception& e) {
			std::cerr << "[YComm] Exception: " << e.what() << std::endl;

			_acceptor->cancel();
			_acceptor->close();
			delete _acceptor;
			_acceptor = NULL;
			success = false;
		}	
	}

	_startServer = success;
	
	return success;
}

void
YTcpServer::handle_accept(YTcpSession* new_session, const boost::system::error_code& error)
{
	if (!error)
	{
		// 새로운 session을 set으로 관리
		_setSession.insert(new_session);

		_c_event->onConnected(*new_session);
		new_session->start();

		accept();
	}
	else
	{
		if(!_startServer)
			std::cerr << "[YComm] socket accept failed!" << std::endl;

		// accept가 실패했으면 서버는 동작중이 아닌 것이다.
		_startServer = false;
				
		if(_acceptor != NULL)
		{
			_acceptor->close();
			delete _acceptor;
			_acceptor = NULL;
		}		

		// 아직 연결이 되지 않은 session이므로 직접 지워준다.
		delete new_session;
	}	
}

bool
YTcpServer::stopServer()
{
	bool success = false;

	std::set<YTcpSession*>::iterator itr;
//	for(itr = _setSession.begin(); itr != _setSession.end(); itr++)
	BOOST_FOREACH(YTcpSession* session, _setSession)
	{
//		YTcpSession* session = *itr;

		if(!session->disconnect())
			std::cerr << "[YComm] TCP session disconnect failed" << std::endl;
		// session을 disconnect하면 자동으로 session이 삭제된다.
		// 여기서 session을 지우면 error가 난다.
		// 연결된 tcp session 객체는 session 스스로 지운다.
	}
	_setSession.clear();

	// acceptor를 삭제한다.
	if(_acceptor != NULL)
	{
		// 여기서는 async_accept를 취소하기만 하면 handle_accept에서 삭제가 이루어진다.
		_acceptor->cancel();
	}	

	return success;
}

}	// namespace ycomm
}	// namespace yolo
