#ifndef _YOLO_NETWORK_MANAGER_H_
#define _YOLO_NETWORK_MANAGER_H_

#include <yolo/YComm/YComm.h>
#include <yolo/YComm/YTcpClient.h>
#include <yolo/YComm/YTcpServer.h>
#include <yolo/YComm/YUdp.h>
#include <yolo/YComm/YTcpConnectionEvent.h>
#include <yolo/YComm/YTcpReceiveDataEvent.h>
#include <yolo/YComm/YUdpReceiveDatagramEvent.h>
#include <yolo/YFramework/YNetworkMessageListener.h>
#include <list>
#include <map>
#include <string>

using namespace std;
using namespace yolo;
using namespace yolo::ycomm;


namespace yolo
{
const uint OPCODE_HEARTBEAT = 100;

class YHeartbeatManager;

class YNetworkManager : public YTcpConnectionEvent, public YTcpReceiveDataEvent, public YUdpReceiveDatagramEvent
{
private:
    struct StreamInfo
    {
	YTcpSession*	session;
	byte*		msgHeader;
	byte*		buffer;
	int		index;
	bool		isReadHeader;
	bool		isConnected;

	StreamInfo() {
	    session = NULL;
	    msgHeader = NULL;
	    buffer = NULL;
	    index = 0;
	    isReadHeader = false;
	    isConnected = false;
	}

	~StreamInfo() {
	    if(msgHeader != NULL)
	    delete[] msgHeader;
	    if(buffer != NULL)
	    delete[] buffer;
	}
    };

    YNetworkManager(){}

public:
    YNetworkManager(string configFilePath);
    ~YNetworkManager();

    // overrides
    virtual void onConnected(YTcpSession& session);
    virtual void onDisconnected(YTcpSession& session);
    virtual void onReceiveData(YTcpSession& session);
    virtual void onReceiveDatagram(string remoteaddr, ushort port, byte* data, ushort length);

    void addNetworkMessageListener(uint opcode, YNetworkMessageListener* listener);

    bool start();
    void stop();

    void sendUdpDatagram(uint opcode, byte* data, uint length);
    void sendTcpPacket(int handle, uint opcode, byte* data, uint length);

    inline void setHeartbeatManager(YHeartbeatManager* hb) { _heartbeat = hb; }

private:
    void readConfiguration(string filepath);
    void dispatchMessage(uint opcode, byte* data, uint length);


private:
    YHeartbeatManager*				_heartbeat;
    map<uint, list<YNetworkMessageListener*>>	_listenerMap;

    StreamInfo					_streamInfo;
    map<int, StreamInfo>			_streamInfoMap;

    // tcp/udp connection info
    bool					_server;
    string					_tcpAddr;
    string					_udpAddr;
    string					_udpPeerAddr;
    string					_hbAddr;
    string					_hbPeerAddr;
    ushort					_tcpPort;
    ushort					_udpPort;
    ushort					_udpPeerPort;
    ushort					_hbPort;
    ushort					_hbPeerPort;

    // Network objects
    YComm*					_comm;
    YTcpClient*					_tcpClient;
    YTcpServer*					_tcpServer;
    YUdp*					_udp;
    YUdp*					_hb;

    bool					_started;
};

}


#endif
