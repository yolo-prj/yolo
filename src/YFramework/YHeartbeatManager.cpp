#include <yolo/YFramework/YHeartbeatManager.h>


namespace yolo
{

YHeartbeatManager::YHeartbeatManager()
{
    _connLostListener = nullptr;
    _networkManager = nullptr;
    _sendHeartbeatThread = nullptr;
    _continueSend = false;
}

YHeartbeatManager::~YHeartbeatManager()
{
    stopHeartbeat();
}

void
YHeartbeatManager::receivedHeartbeat(string addr, ushort port, byte* data, uint length)
{
    cout << "received heartbeat" << endl;
    // reset timeout timer


}

void
YHeartbeatManager::startHeartbeat()
{
    _continueSend = true;
    _sendHeartbeatThread = new boost::thread( bind(&YHeartbeatManager::sendHeartbeatThread, this) );
}

void
YHeartbeatManager::stopHeartbeat()
{
    if(_sendHeartbeatThread != nullptr) {
	_continueSend = false;
	_sendHeartbeatThread->join();
	delete _sendHeartbeatThread;
    }
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
	    _networkManager->sendUdpDatagram(OPCODE_HEARTBEAT, heartbeat, sizeof(heartbeat));
	    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}
    }
}


} // namespace yolo
