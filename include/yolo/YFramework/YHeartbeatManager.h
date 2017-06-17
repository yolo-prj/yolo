#ifndef _YOLO_HEARTBEAT_MANAGER_H_
#define _YOLO_HEARTBEAT_MANAGER_H_

#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YConnectionLostListener.h>
#include <boost/thread.hpp>

namespace yolo
{
class YNetworkManager;

class YHeartbeatManager
{
public:
    YHeartbeatManager();
    ~YHeartbeatManager();

    inline void setConnectionLostListener(YConnectionLostListener* listener) {
	this->_connLostListener = listener;
    }
    inline void setNetworkManager(YNetworkManager* manager) {
	this->_networkManager = manager;
    }

    void startHeartbeat();
    void stopHeartbeat();
    void receivedHeartbeat(string remoteaddr, ushort port, byte* data, uint length);

private:
    void sendHeartbeatThread();

private:
    YConnectionLostListener*	_connLostListener;
    YNetworkManager*		_networkManager;

    boost::thread*		_sendHeartbeatThread;
    bool			_continueSend;
};

}
#endif
