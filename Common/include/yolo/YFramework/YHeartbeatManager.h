#ifndef _YOLO_HEARTBEAT_MANAGER_H_
#define _YOLO_HEARTBEAT_MANAGER_H_

#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YConnectionLostListener.h>
#include <boost/thread.hpp>
#include <map>
#include <set>

using namespace std;

namespace yolo
{
class YNetworkManager;

class YHeartbeatManager
{
public:
    YHeartbeatManager();
    YHeartbeatManager(YNetworkManager* manager);
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
    void checkHeartbeatThread();


private:
    YConnectionLostListener*	_connLostListener;
    YNetworkManager*		_networkManager;

    boost::thread*		_sendHeartbeatThread;
    boost::thread*		_checkHeartbeatThread;
    bool			_continueSend;
    bool			_continueCheck;
    bool			_heartbeatReceived;

    map<string, uint>		_timeoutMap;
    boost::mutex		_mutex;
    bool			_started;

    set<string>			_lostSet;
};

}
#endif
