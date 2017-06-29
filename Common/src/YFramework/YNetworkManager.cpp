#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YHeartbeatManager.h>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


namespace yolo
{

YNetworkManager::YNetworkManager(string configFilePath)
{
    _heartbeat = nullptr;
    _tcpClient = nullptr;
    _tcpServer = nullptr;
    _udp = nullptr;
    _hb = nullptr;
    _comm = nullptr;
    _server = false;

    _started = false;

    readConfiguration(configFilePath);
}

YNetworkManager::~YNetworkManager()
{
    if(_started)
	stop();
}

void
YNetworkManager::readConfiguration(string filepath)
{
    const string SECTION_HEARTBEAT  = "Heartbeat.";
    const string SECTION_TCP	    = "TCP.";
    const string SECTION_UDP	    = "UDP.";
    const string KEY_SERVER	    = "Server";
    const string KEY_IPADDR	    = "Address";
    const string KEY_PORT	    = "Port";
    const string KEY_PEER_IPADDR    = "PeerAddress";
    const string KEY_PEER_PORT	    = "PeerPort";
    const string KEY_OPCODE	    = "Opcode";


    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(filepath, pt);

    string key;

    key = SECTION_HEARTBEAT;
    key.append(KEY_IPADDR);
    _hbAddr = pt.get<string>(key);

    key = SECTION_HEARTBEAT;
    key.append(KEY_PORT);
    _hbPort = pt.get<ushort>(key);

    key = SECTION_HEARTBEAT;
    key.append(KEY_PEER_IPADDR);
    _hbPeerAddr = pt.get<string>(key);

    key = SECTION_HEARTBEAT;
    key.append(KEY_PEER_PORT);
    _hbPeerPort = pt.get<ushort>(key);
    cout << "heartbeat addr : " << _hbAddr << ", port : " << _hbPort << endl;

    key = SECTION_TCP;
    key.append(KEY_SERVER);
    _server = pt.get<string>(key) == "true" ? true : false;

    key = SECTION_TCP;
    key.append(KEY_IPADDR);
    _tcpAddr = pt.get<string>(key);

    key = SECTION_TCP;
    key.append(KEY_PORT);
    _tcpPort = pt.get<ushort>(key);
    cout << "tcp addr : " << _tcpAddr << ", port : " << _tcpPort << ", server : " << _server << endl;

    key = SECTION_UDP;
    key.append(KEY_IPADDR);
    _udpAddr = pt.get<string>(key);

    key = SECTION_UDP;
    key.append(KEY_PORT);
    _udpPort = pt.get<ushort>(key);

    key = SECTION_UDP;
    key.append(KEY_PEER_IPADDR);
    _udpPeerAddr = pt.get<string>(key);

    key = SECTION_UDP;
    key.append(KEY_PEER_PORT);
    _udpPeerPort = pt.get<ushort>(key);

    cout << "udp addr : " << _udpAddr << ", port : " << _udpPort << endl;

}

void
YNetworkManager::addNetworkMessageListener(uint opcode, YNetworkMessageListener* listener)
{
    auto itr = _listenerMap.find(opcode);
    
    if(itr != _listenerMap.end())
    {
	itr->second.push_back(listener);
    }
    else
    {
	list<YNetworkMessageListener*> listenerList;
	listenerList.push_back(listener);

	_listenerMap.insert( make_pair(opcode, listenerList) );
    }
}

void
//YNetworkManager::dispatchMessage(uint opcode, byte* data, uint length)
YNetworkManager::dispatchMessage(uint opcode, boost::shared_ptr<byte[]> data, uint length)
{
    auto itr = _listenerMap.find(opcode);
    
    /*
    uint o;
    uint len;
    memcpy(&o, data.get(), 4);
    memcpy(&len, data.get()+4, 4);

    cout << "opcode : " << opcode << ", o : " << o << ", len: " << len<< endl;
    */

    if(itr != _listenerMap.end())
    {
	auto& listenerList = itr->second;

	for(auto listener : listenerList)
	{
	    listener->onReceiveMessage(data.get(), length);
	}
    }
    else
    {
	// It must not be happned.
	cerr << "There is no component to send message for the opcode " << opcode << endl;
    }

}

bool
YNetworkManager::start()
{
    bool success = true;

    _comm = &YComm::getInstance();

    struct TcpException{};
    struct UdpException{};

    try
    {
	if(_server)
	{
	    //_tcpServer = new YTcpServer(_tcpAddr, _tcpPort);
	    _tcpServer = new YTcpServer(_tcpPort);
	    _tcpServer->setTcpConnectionEvent(this);
	    _tcpServer->setTcpReceiveDataEvent(this);

	    if(!_comm->registerTcpServer(_tcpServer))
		throw TcpException();
	}
	else
	{
	    _tcpClient = new YTcpClient(_tcpAddr, _tcpPort, true);
	    _tcpClient->setTcpConnectionEvent(this);
	    _tcpClient->setTcpReceiveDataEvent(this);

	    if(!_comm->registerTcpClient(_tcpClient))
		throw TcpException();
	}

	// UDP setup
	_udp = new YUdp(_udpAddr, _udpPort, UNICAST);
	_hb = new YUdp(_hbAddr, _hbPort, UNICAST);

	_udp->setReceiveDatagramEvent(this);
	_hb->setReceiveDatagramEvent(this);

	if(!_comm->registerUdp(_udp))
	    throw UdpException();

	if(!_comm->registerUdp(_hb))
	    throw UdpException();

	_started = true;
    } catch(TcpException& e) {
	success = false;

	if(_tcpServer != nullptr) {
	    _tcpServer = nullptr;
	} else if(_tcpClient != nullptr) {
	    _tcpClient = nullptr;
	}
    } catch(UdpException& e) {
	success = false;

	if(_tcpServer != nullptr) {
	    _comm->removeTcpServer(_tcpServer);
	    _tcpServer = nullptr;
	} else if(_tcpClient != nullptr) {
	    _comm->removeTcpClient(_tcpClient);
	    _tcpClient = nullptr;
	}
    }

    return success;
}

void
YNetworkManager::stop()
{
    _started = false;

    _comm = &YComm::getInstance();
    _comm->stop();

    if(_server && _tcpServer != nullptr) {
	try{
	    _comm->removeTcpServer(_tcpServer);
	} catch(...){}
	_tcpServer = nullptr;
    } else if(!_server && _tcpClient != nullptr) {
	try{
	    _comm->removeTcpClient(_tcpClient);
	} catch(...){}
	_tcpClient = nullptr;
    }

    if(_udp != nullptr) {
	try {
	    _comm->removeUdp(_udp);
	} catch(...){}
	_udp = nullptr;
    }

    if(_hb != nullptr) {
	try{
	    _comm->removeUdp(_hb);
	} catch(...){}
	_hb = nullptr;
    }

    cout << "[YNetworkManager] stopped" << endl;
}

void
YNetworkManager::onConnected(YTcpSession& session)
{
    StreamInfo info;
    info.buffer = NULL;
    info.index = 0;
    info.session = &session;
    info.isReadHeader = false;
    info.isConnected = true;

    _streamInfoMap.insert( make_pair(session.getSocketHandle(), info) );

    // call-back connection info
    for(auto listenerList : _listenerMap) {
	for(auto listener : listenerList.second) {
	    listener->onTcpClientConnected(session.getSocketHandle(), session.getPeerAddress(), session.getPeerPort());
	}
    }

    cout << "[YNetworkManager] TCP connected" << endl;
}

void
YNetworkManager::onDisconnected(YTcpSession& session)
{
    _streamInfoMap.erase( session.getSocketHandle() );

    // call-back disconnection info
    for(auto listenerList : _listenerMap) {
	for(auto listener : listenerList.second) {
	    listener->onTcpClientDisconnected(session.getSocketHandle(), session.getPeerAddress(), session.getPeerPort());
	}
    }

    cout << "[YNetworkManager] TCP DISconnected" << endl;
}

void
YNetworkManager::onReceiveData(YTcpSession& session)
{
    //cout << "YNetworkManager::onReceiveData" << endl;

    int handle = session.getSocketHandle();
    auto itr = _streamInfoMap.find(handle);
    if(itr != _streamInfoMap.end()) {
	StreamInfo& info = itr->second;

	uint msgLength = 0;
	uint opcode = 0;

	uint opcodeSize = sizeof(uint);
	uint msgLengthSize = sizeof(uint);

	int bytes_read = 0;
	int new_bytes_read = 0;

	const int msgHeaderSize = sizeof(uint) * 2;
	const int MAX_LENGTH = 1024*1024*1024;

	while(true)
	{
	    if(info.index == 0)
	    {
		if(info.buffer != NULL)
		{
		    //delete[] info.buffer;
		    //info.buffer = NULL;		    
		}
		else
		{
		    // Buffer is NULL
		}

		if(info.msgHeader != NULL)
		{
		    //delete[] info.msgHeader;
		    //info.msgHeader = NULL;
		}		

		//info.msgHeader = new byte[msgHeaderSize];
		boost::shared_ptr<byte[]> newptr = boost::make_shared<byte[]>(msgHeaderSize);
		info.msgHeader = newptr;
		memset(info.msgHeader.get(), 0, msgHeaderSize);
	    }

	    if(info.index < msgHeaderSize)
	    {
		bytes_read = info.index;
	    }
	    else
	    {
		bytes_read = 0;
	    }

	    new_bytes_read = 0;

	    if(!info.isReadHeader)
	    {		    
		do
		{
		    new_bytes_read = session.recv((unsigned char*)info.msgHeader.get() + bytes_read, msgHeaderSize - bytes_read);

		    if(new_bytes_read > 0)
		    {
			bytes_read += new_bytes_read;
			info.index = bytes_read;
		    }
		    else
		    {
			// If failed to receive data, can be receive on next turn.
			return;
		    }
		} while(bytes_read != msgHeaderSize);
	    }

	    info.isReadHeader = true;

	    // Message ID
	    memcpy(&opcode, info.msgHeader.get(), opcodeSize );

	    // Message Length
	    msgLength = 0;
	    memcpy(&msgLength, info.msgHeader.get() + opcodeSize , msgLengthSize);   

	    if(msgLength < 0 || msgLength > MAX_LENGTH)
	    {
		break;
	    }

	    if(info.buffer == NULL)
	    {
		//info.buffer = new byte[msgLength];
        boost::shared_ptr<byte[]> newptr = boost::make_shared<byte[]>(msgLength+1024);
		info.buffer = newptr;
        memset(info.buffer.get(), 0, msgLength+1024);
	    }	    
	    memcpy(info.buffer.get(), info.msgHeader.get(), msgHeaderSize);

	    bytes_read = info.index - msgHeaderSize;
	    new_bytes_read = 0;

	    if(msgLength > msgHeaderSize)
	    {
		do
		{
		    new_bytes_read = session.recv((unsigned char*)info.buffer.get() + bytes_read + msgHeaderSize, msgLength - bytes_read - msgHeaderSize);

		    if(new_bytes_read > 0)
		    {
			bytes_read += new_bytes_read;
			info.index = bytes_read + msgHeaderSize;
		    }
		    else
		    {
			// If failed to receive data, can be receive on next turn.
			return;
		    }
		} while(bytes_read + msgHeaderSize != msgLength);
	    }		

	    info.isReadHeader = false;
	    info.index = 0;

	    // opcode processing & call-back received message
	    /*
	    auto itrList = _listenerMap.find(opcode);
	    if(itrList != _listenerMap.end()) {
		auto listenerList = itrList->second;

		for(auto listener : listenerList) {
		    listener->onReceiveMessage(info->buffer, msgLength);
		}
	    }
	    */

	    boost::shared_ptr<byte[]> dataPtr = boost::make_shared<byte[]>(msgLength);
	    memcpy(dataPtr.get(), info.buffer.get(), msgLength);

	    //dispatchMessage(opcode, info->buffer, msgLength);
	    dispatchMessage(opcode, dataPtr, msgLength);
	    //boost::thread t(boost::bind(&YNetworkManager::dispatchMessage, this, opcode, dataPtr, msgLength));
	}

    }
}

void
YNetworkManager::onReceiveDatagram(string remoteaddr, ushort port, byte* data, ushort length)
{
    uint opcode;
    memcpy(&opcode, data, sizeof(uint));

    if(opcode != OPCODE_HEARTBEAT)
    {
	auto itr = _listenerMap.find(opcode);
	if(itr != _listenerMap.end())
	{
	    auto listenerList = itr->second;

	    // call-back data receive
	    for(auto listener : listenerList) {
		listener->onReceiveMessage(data, length);
	    }
	}
    }
    else
    {
	// call-back data receive
	// Heartbeat
	if(_heartbeat != nullptr) {
	    if(remoteaddr == _hbPeerAddr && port == _hbPeerPort)
		_heartbeat->receivedHeartbeat(remoteaddr, port, data, length);
	}
    }
}

void
YNetworkManager::sendUdpDatagram(uint opcode, byte* data, uint length)
{
    if(_started)
    {
	try {
	    if(opcode == OPCODE_HEARTBEAT)
	    {
		_hb->sendTo(data, length, _hbPeerAddr, _hbPeerPort);
	    }
	    else
	    {
		_udp->sendTo(data, length, _udpPeerAddr, _udpPeerPort);
	    }
	} catch(...) {
	}
    }
}

void
YNetworkManager::sendUdpDatagram(uint opcode, string addr, ushort port, byte* data, uint length)
{
    if(_started)
    {
	try {
	    if(opcode == OPCODE_HEARTBEAT)
	    {
		_hb->sendTo(data, length, addr, port);
	    }
	    else
	    {
		_udp->sendTo(data, length, addr, port);
	    }
	} catch(...) {
	}
    }
}

void
YNetworkManager::sendTcpPacket(int handle, uint opcode, byte* data, uint length)
{
    if(_started)
    {
	if(handle != 0)
	{
	    auto itr = _streamInfoMap.find(handle);
	    
	    if(itr != _streamInfoMap.end()) {
		YTcpSession* session = itr->second.session;

		session->send(data, length);
	    }
	}
	else
	{
	    for(auto stream : _streamInfoMap) {
		YTcpSession* session = stream.second.session;

		session->send(data, length);
	    }
	}
    }
}

/*
void
YNetworkManager::sendTcpPacket(int handle, uint opcode, boost::shared_ptr<byte[]> data, uint length)
{
    if(_started)
    {
	if(handle != 0)
	{
	    auto itr = _streamInfoMap.find(handle);
	    
	    if(itr != _streamInfoMap.end()) {
		YTcpSession* session = itr->second.session;

		session->send(data.get(), length);
	    }
	}
	else
	{
	    for(auto stream : _streamInfoMap) {
		YTcpSession* session = stream.second.session;

		session->send(data.get(), length);
	    }
	}
    }
}
*/

}
