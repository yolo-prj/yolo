#pragma once
#include <yolo/YComm/YCommCommon.h>
#include <yolo/YComm/YTcpConnectionEvent.h>
#include <yolo/YComm/YTcpSession.h>
#include <set>
#include <boost/asio.hpp>


namespace yolo
{
namespace ycomm
{

class YTcpConnectionEvent;
class YTcpReceiveDataEvent;

class YCOMMDLL_API YTcpServer : public YTcpConnectionEvent
{
public:
	YTcpServer(std::string serverAddr, unsigned short serverPort);	
	YTcpServer(unsigned short serverPort);	

	virtual void				onConnected(YTcpSession& session);
	virtual void				onDisconnected(YTcpSession& session);

private:
	YTcpServer(void){}
	~YTcpServer(void);
	//inline void					setAsioIoService(boost::asio::io_service* s) { _io_service = s; }

	bool						accept();
	bool						stopServer();

	void						handle_accept(YTcpSession* new_session, const boost::system::error_code& error);

public:	
	inline void					setTcpConnectionEvent(YTcpConnectionEvent* e) { _c_event = e; }
	inline void					setTcpReceiveDataEvent(YTcpReceiveDataEvent* e) { _r_event = e; }

	inline void					setBufferSize(size_t s) { _bufferSize = s; }
	inline size_t				getBufferSize() { return _bufferSize; }

private:
	friend class YComm;
	//boost::asio::io_service&	_io_service;
	boost::asio::ip::tcp::acceptor*	_acceptor;

	YTcpConnectionEvent*		_c_event;
	YTcpReceiveDataEvent*		_r_event;

	std::set<YTcpSession*>		_setSession;

	std::string					_address;
	unsigned short				_port;
	bool						_startServer;

	size_t						_bufferSize;
};

}	// namespace ycomm
}	// namespace yolo
