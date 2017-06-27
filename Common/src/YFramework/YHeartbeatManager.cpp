#include <yolo/YFramework/YHeartbeatManager.h>
#include <list>

namespace yolo
{

YHeartbeatManager::YHeartbeatManager()
{
    _connLostListener = nullptr;
    _networkManager = nullptr;
    _sendHeartbeatThread = nullptr;
    _continueSend = false;
    _continueCheck = false;
    _heartbeatReceived = false;
    _checkHeartbeatThread = nullptr;
    _started = false;
}

YHeartbeatManager::YHeartbeatManager(YNetworkManager* manager)
{
    _connLostListener = nullptr;
    _networkManager = nullptr;
    _sendHeartbeatThread = nullptr;
    _continueSend = false;
    _continueCheck = false;
    _heartbeatReceived = false;
    _checkHeartbeatThread = nullptr;
    setNetworkManager(manager);
    _started = false;
}

YHeartbeatManager::~YHeartbeatManager()
{
    if(_started)
	stopHeartbeat();
}

void
YHeartbeatManager::receivedHeartbeat(string addr, ushort port, byte* data, uint length)
{
//    cout << "[HeartbeatManager] received heartbeat" << endl;
    // reset timeout timer

    char buf[100];
    string key;

    sprintf(buf, "%s:%d", addr.c_str(), port);
    key = buf;

    auto itr =  _timeoutMap.find(key);
    if(itr == _timeoutMap.end() )
    {
	// start check thread
	if(_checkHeartbeatThread == nullptr)
	{
	    cout << "start heartbeat check!!" << endl;
	    _continueCheck = true;
	    _checkHeartbeatThread = new boost::thread( bind(&YHeartbeatManager::checkHeartbeatThread, this) );
	}

	boost::mutex::scoped_lock lock(_mutex);
	_timeoutMap.insert( make_pair(key, 0) );

	// if received from lost peer, send reconnected message to listener
	auto itrLost = _lostSet.find(key);
	if(itrLost != _lostSet.end())
	{
	    _lostSet.erase(itrLost);

	    if(_connLostListener != nullptr) {
		_connLostListener->reconnected(addr, port);
	    }
	}
    }
    else
    {
	boost::mutex::scoped_lock lock(_mutex);
	itr->second = 0;
    }
}

void
YHeartbeatManager::startHeartbeat()
{
    _continueSend = true;
    _sendHeartbeatThread = new boost::thread( bind(&YHeartbeatManager::sendHeartbeatThread, this) );
    _started = true;
}

void
YHeartbeatManager::stopHeartbeat()
{
    if(_sendHeartbeatThread != nullptr) {
	_continueSend = false;
	if(_sendHeartbeatThread->joinable())
	    _sendHeartbeatThread->join();
	delete _sendHeartbeatThread;
	_sendHeartbeatThread = nullptr;
    }

    if(_checkHeartbeatThread != nullptr) {
	_continueCheck = false;
	if(_checkHeartbeatThread->joinable())
	    _checkHeartbeatThread->join();
	delete _checkHeartbeatThread;
	_checkHeartbeatThread = nullptr;
    }
    _started = false;
    cout << "[YHeartbeatManager] stop" << endl;
}

void
YHeartbeatManager::sendHeartbeatThread()
{
    byte heartbeat[12] = {0};
    uint length = sizeof(heartbeat);

    memcpy(heartbeat, &OPCODE_HEARTBEAT, sizeof(uint));
    memcpy(heartbeat + sizeof(uint), &length, sizeof(uint));

    while(_continueSend)
    {
	// send heartbeat
	if(_networkManager) {
	    //_networkManager->sendUdpDatagram(OPCODE_HEARTBEAT, heartbeat, sizeof(heartbeat));
	    string addr;
	    ushort port;

	    if(_timeoutMap.size() == 0)
	    {
		_networkManager->sendUdpDatagram(OPCODE_HEARTBEAT, heartbeat, sizeof(heartbeat));
	    }
	    else
	    {
		for(auto e : _timeoutMap) {
		    string key = e.first;
		    addr = key.substr(0, key.find(":"));
		    port = (ushort)atoi( key.substr(key.find(":") + 1).c_str() );

		    _networkManager->sendUdpDatagram(OPCODE_HEARTBEAT, addr, port, heartbeat, sizeof(heartbeat));
		}
	    }

	    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}
    }
}

void
YHeartbeatManager::checkHeartbeatThread()
{
    const uint period = 1000;		// ms
    const uint loopPeriod = 100;	// ms
    const uint timeout = period * 3;	//ms
    uint counter = 0;

    while(_continueCheck)
    {
	if(counter != 0 && (counter % period == 0))
	{
	    boost::mutex::scoped_lock lock(_mutex);
	    list<string> removed;
	    for(auto& e : _timeoutMap) {
		e.second += period;

		if(e.second >= timeout) {
		    // notify connection lost
		    if(_connLostListener != nullptr) {
			string key, addr;
			ushort port;

			key = e.first;
			addr = key.substr(0, key.find(":"));
			port = atoi(key.substr(key.find(":") + 1).c_str());

			_connLostListener->connectionLost(addr, port);
		    }

		    removed.push_back(e.first);
		    _lostSet.insert(e.first);
		}
	    }

	    for(auto& e : removed) {
		auto itr = _timeoutMap.find(e);
		_timeoutMap.erase(itr);
	    }
	}

	counter += loopPeriod;

	boost::this_thread::sleep(boost::posix_time::milliseconds(loopPeriod));
    }
}


} // namespace yolo
