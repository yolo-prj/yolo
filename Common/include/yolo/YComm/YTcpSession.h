#pragma once
#include <yolo/YComm/YCommCommon.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/pool/pool.hpp>
#include <string>
#include <deque>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

using namespace std;

namespace yolo
{
namespace ycomm
{

class YTcpConnectionEvent;
class YTcpReceiveDataEvent;

class YCOMMDLL_API YTcpSession
{
private:
	YTcpSession(boost::asio::io_service& io_service);
	~YTcpSession(void);

	friend class YTcpServer;
	friend class YTcpClient;

public:	
	bool			disconnect();
	inline bool		getConnected() { return _connected; }
	
	std::string		getPeerAddress();
	unsigned short	getPeerPort();
	std::string		getLocalAddress();
	unsigned short	getLocalPort();
	boost::asio::ip::tcp::socket&	getSocket() { return *_tcp_socket; }
	int				getSocketHandle();
	void			setBufferSize(size_t s) { _bufferSize = s; }

	void			registerConnectionEventCallback(YTcpConnectionEvent*);
	void			registerReceiveDataEventCallback(YTcpReceiveDataEvent*);

	size_t			send(unsigned char* data, size_t dataLength);
	size_t			recv(unsigned char* data, size_t dataLength);

	void			start();

private:
	bool			connect(std::string serverAddress, unsigned short serverPort, unsigned int timeout = 0);	

	void			readSome();

	void			handle_connect(const boost::system::error_code& ec);
	void			handle_read(const boost::system::error_code& error, size_t bytes_transferred, unsigned char* data);
	void			handle_send(const boost::system::error_code& error, size_t bytes_transferred);
	void			handle_write(const boost::system::error_code& error);

	void			block_thread(unsigned int timeout);

        void                    readSomeThread();
public:
	unsigned int					_number;

private:
	bool							_connected;
	//boost::asio::ip::tcp::socket	_tcp_socket;
	boost::shared_ptr<boost::asio::ip::tcp::socket>	_tcp_socket;
	deque<YTcpConnectionEvent*>		_connectionEvent;
	YTcpReceiveDataEvent*			_receiveDataEvent;
	boost::asio::io_service&		_io_service;
        boost::asio::strand                     _strand;
        boost::asio::io_service::work           _work;
        boost::thread*                          _readThread;
	
        bool                                    _receivedSome;
        bool                                    _continueRead;
	bool					_asyncConnectResult;
	bool					_asyncWaitTimeout;

	size_t					_bufferSize;	
	
	struct BufferInfo {
		unsigned char*			data;
		size_t			bytesTransferred;
		unsigned int	dataOffset;

		BufferInfo() {
			data = NULL;
			bytesTransferred = 0;
			dataOffset = 0;
		}
	};
	std::deque<BufferInfo>				_dataQueue;
	boost::pool<>*						_bufferPool;

	bool								_valid;

	boost::mutex						_mutex;
};

}	// namespace ycomm
}	// namespace yolo
