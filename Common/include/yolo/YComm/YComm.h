#pragma once
#include <yolo/YComm/YCommCommon.h>
#include <yolo/YComm/YTcpConnectionEvent.h>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <set>

namespace yolo
{
namespace ycomm
{


class YTcpServer;
class YTcpClient;
class YTcpSession;
class YUdp;

class YCOMMDLL_API YComm : public YTcpConnectionEvent
{
public:
	YComm();
	~YComm();

	static YComm& getInstance();

	bool registerTcpServer(YTcpServer*);
	bool registerTcpClient(YTcpClient*, unsigned int timeout = 0);	// timeout : millisec
	bool removeTcpServer(YTcpServer*);
	bool removeTcpClient(YTcpClient*);
	bool registerUdp(YUdp*);
	bool removeUdp(YUdp*);

	void stop();

	inline boost::asio::io_service& getIOService() { return _io_service; }	

	virtual void onConnected(YTcpSession& session);
	virtual void onDisconnected(YTcpSession& session);

private:
	// Singleton Object
	static void createInstance();
	static void releaseInstance();
	static void onDeadReference();
	static void killYComm();

	// run loop
	void runLoop(void* argv);

private:
	static YComm*			_singleton;		// singleton
	static bool				_destroyed;		// singleton object destroyed or not
	boost::asio::io_service	_io_service;	// boost asio io_service object

	std::set<YTcpClient*>	_setClient;		// YTcpClient object set
	std::set<YTcpServer*>	_setServer;		// YTcpServer object set
	std::set<YUdp*>			_setUdp;		// YUdp object set
	
	//NThreadPool*			m_pThreadPool;		
	
	bool					_continueLoop;

};

}	// namespace ycomm
}	// namespace yolo
