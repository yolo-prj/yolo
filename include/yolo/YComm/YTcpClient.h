#pragma once
#include <yolo/YComm/YCommCommon.h>
#include <yolo/YComm/YTcpConnectionEvent.h>
#include <yolo/YComm/YTcpSession.h>
#include <string>
#include <boost/asio.hpp>

namespace yolo
{
class NThreadPool;

namespace ycomm
{

class YTcpConnectionEvent;
class YTcpReceiveDataEvent;

class YCOMMDLL_API YTcpClient : public YTcpConnectionEvent
{
public:
	YTcpClient(std::string serverAddr, unsigned short serverPort);	
	YTcpClient(std::string serverAddr, unsigned short serverPort, bool isTryToReconnect);
	~YTcpClient(void);

	virtual void				onConnected(YTcpSession& session);
	virtual void				onDisconnected(YTcpSession& session);

private:	
	YTcpClient(){};	
	//inline void					setAsioIoService(boost::asio::io_service* s) { _io_service = s; }

	bool						connect(unsigned int timeout = 0);
	bool						disconnect();	

	void						reconnectThread(void* arg);

public:
	inline void					setTcpConnectionEvent(YTcpConnectionEvent* e) { _c_event = e; }
	inline void					setTcpReceiveDataEvent(YTcpReceiveDataEvent* e) { _r_event = e; }

	inline bool					isDisconnecting() { return _disconnecting; }

	inline YTcpSession*			getTcpSession() { return _session; }

	inline void					setBufferSize(size_t s) { _bufferSize = s; }
	inline size_t				getBufferSize() { return _bufferSize; }

	size_t						send(unsigned char* data, size_t dataLength);
	size_t						recv(unsigned char* data, size_t dataLength);
	
private:
	friend class YComm;

	//boost::asio::io_service&	_io_service;

	YTcpConnectionEvent*		_c_event;
	YTcpReceiveDataEvent*		_r_event;

	YTcpSession*				_session;

	bool						_connected;

	std::string					_serverAddress;
	unsigned short				_serverPort;

	size_t						_bufferSize;

	bool						_isTryToReconnect;
	bool						_disconnecting;
};

}	// namespace ycomm
}	// namespace yolo
