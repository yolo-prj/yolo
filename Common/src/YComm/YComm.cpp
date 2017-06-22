// YComm.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include <yolo/YComm/YComm.h>
#include <yolo/YComm/YTcpClient.h>
#include <yolo/YComm/YTcpServer.h>
#include <yolo/YComm/YUdp.h>
#include <boost/foreach.hpp>
#include <algorithm>
#include <boost/thread.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace yolo
{
namespace ycomm
{

YComm* YComm::_singleton = NULL;
bool YComm::_destroyed = false;

YComm::YComm()
{	
    _continueLoop = true; 
    //auto bf = boost::bind(&YComm::runLoop, this, _1);
}

YComm::~YComm()
{	
    stop();

    std::set<YTcpClient*>::iterator itrClient;
    std::set<YTcpServer*>::iterator itrServer;
    std::set<YUdp*>::iterator itrUdp;
	    
    BOOST_FOREACH(YTcpClient* client, _setClient) {
	if(client != NULL)
	    delete client;
    }
    BOOST_FOREACH(YTcpServer* server, _setServer) {
	if(server != NULL)
	    delete server;
    }
    BOOST_FOREACH(YUdp* udp, _setUdp) {
	if(udp != NULL)
	    delete udp;
    }	

    cout << "[YComm] destructor" << endl;
}

void
YComm::runLoop(void* argv)
{
    try
    {
	while(_continueLoop) {
	    if(_io_service.run() == 0)
	    {
		boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
	    }
	}
    }
    catch(std::exception& e)
    {
	    _continueLoop = false;
	    tcerr << _T("[YComm] an exception occurred : ") << e.what() << endl;
    }
}

void
YComm::stop()
{
    _continueLoop = false;
    _io_service.stop();	
}

void
YComm::createInstance()
{
    static YComm theInstance;

    _singleton = &theInstance;
}

void
YComm::releaseInstance()
{
    _singleton = NULL;
    _destroyed = true;
}

void
YComm::onDeadReference()
{
    createInstance();

    new(_singleton) YComm;

    atexit(killYComm);

    _destroyed = false;
}

void
YComm::killYComm()
{
    _singleton->~YComm();
}

YComm&
YComm::getInstance()
{
    if(!_singleton)
    {
	if(_destroyed)
	{
	    onDeadReference();
	}
	else
	{
	    createInstance();
	}		
    }

    return *_singleton;
}

bool
YComm::registerTcpClient(YTcpClient* client, unsigned int timeout)
{
    bool success = false;

    std::set<YTcpClient*>::iterator itr;

    itr = _setClient.find(client);

    if(client != NULL && itr == _setClient.end())
    {
	//client->setAsioIoService(&_io_service);
	success = client->connect(timeout);

	if(success) {
	    _setClient.insert(client);			
	}
    }

    return success;
}

bool
YComm::registerTcpServer(YTcpServer* server)
{
	bool success = false;

	std::set<YTcpServer*>::iterator itr;

	itr = _setServer.find(server);

	if(server!= NULL && itr == _setServer.end())
	{
		//server->setAsioIoService(&_io_service);
		success = server->accept();

		if(success) {
			_setServer.insert(server);			
		}
	}

	return success;
}

bool
YComm::registerUdp(YUdp* udp)
{
	bool success = false;

	std::set<YUdp*>::iterator itr;

	itr = _setUdp.find(udp);

	if(udp!= NULL && itr == _setUdp.end())
	{
		//udp->setAsioIoService(&_io_service);
		success = udp->open();

		if(success) {
			_setUdp.insert(udp);			
		}
	}

	return success;
}

bool
YComm::removeTcpClient(YTcpClient* client)
{
	bool success = false;

	std::set<YTcpClient*>::iterator itr;

	itr = _setClient.find(client);

	if(client != NULL && itr != _setClient.end())
	{
		success = client->disconnect();

		if(success) {
			//_setClient.erase(itr);
			//delete client;
		}
	}

	return success;
}

bool
YComm::removeTcpServer(YTcpServer* server)
{
	bool success = false;

	std::set<YTcpServer*>::iterator itr;

	itr = _setServer.find(server);

	if(server != NULL && itr != _setServer.end())
	{
		success = server->stopServer();

		if(success) {
			_setServer.erase(itr);
			delete server;
		}
	}

	return success;
}

bool
YComm::removeUdp(YUdp* udp)
{
	bool success = false;

	std::set<YUdp*>::iterator itr;

	itr = _setUdp.find(udp);

	if(udp != NULL && itr != _setUdp.end())
	{
		_setUdp.erase(itr);
		delete udp;
		success = true;
	}

	return success;
}

void
YComm::onConnected(YTcpSession& session)
{

}

void
YComm::onDisconnected(YTcpSession& session)
{
	set<YTcpClient*>::iterator itr;

	for(itr = _setClient.begin(); itr != _setClient.end(); itr++)
	{
		YTcpClient* client = *itr;
		YTcpSession* sessionOfClient = client->getTcpSession();

		if(sessionOfClient == &session && client->isDisconnecting())
		{
			_setClient.erase(itr);
			delete client;
			break;
		}
	}
}

}	// namespace ycomm
}	// namespace yolo
