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
}

void
YTcpServer::onDisconnected(YTcpSession& session)
{
	
    std::set<YTcpSession*>::iterator itr;
    itr = _setSession.find(&session);
    if(itr != _setSession.end())
	_setSession.erase(itr);
}

bool
YTcpServer::accept()
{
    bool success = false;

    {
	if(_acceptor == NULL) {
	    boost::asio::io_service& io = YComm::getInstance().getIOService();
	    _acceptor = new boost::asio::ip::tcp::acceptor(io);
	}
	
	try {
	    boost::system::error_code error;

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

		new_session->registerConnectionEventCallback(this);
		new_session->registerConnectionEventCallback(_c_event);
		new_session->registerReceiveDataEventCallback(_r_event);

		_acceptor->async_accept(new_session->getSocket(),
		    boost::bind(&YTcpServer::handle_accept, this, new_session,
		    boost::asio::placeholders::error));

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
	_setSession.insert(new_session);

	_c_event->onConnected(*new_session);
	new_session->start();

	accept();
    }
    else
    {
	if(!_startServer)
	    std::cerr << "[YComm] socket accept failed!" << std::endl;

	_startServer = false;
			
	if(_acceptor != NULL)
	{
	    _acceptor->close();
	    delete _acceptor;
	    _acceptor = NULL;
	}		

	delete new_session;
    }	
}

bool
YTcpServer::stopServer()
{
    bool success = false;

    BOOST_FOREACH(YTcpSession* session, _setSession)
    {
	if(!session->disconnect())
	    std::cerr << "[YComm] TCP session disconnect failed" << std::endl;
    }
    _setSession.clear();

    if(_acceptor != NULL)
    {
//	_acceptor->cancel();
	_acceptor->close();
    }	

    return success;
}

}	// namespace ycomm
}	// namespace yolo
