#include <iostream>
#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YMessageFormatParser.h>
#include <yolo/YFramework/YNetworkMessageListener.h>
#include <yolo/YFramework/YHeartbeatManager.h>


using namespace std;
using namespace yolo;

class TestClient : public YNetworkMessageListener
{
public:
    TestClient(YNetworkManager* manager, YHeartbeatManager* hb) { 
	this->manager = manager; 
	this->hb = hb;

	hb->setNetworkManager(manager);
    } 
    void onReceiveMessage(byte* data, uint length)
    {
    }

    void onTcpClientConnected(int handle, string addr, ushort port)
    {
	this->handle = handle;
	cout << "handle : " << handle << endl;
    }

    void testMessage()
    {
	YMessageFormatParser parser("msgformat.json");
	if(parser.parse())
	{
	    YMessage msg = parser.getMessage(1);

	    uint length = 0;
	    byte* data = msg.serialize(length);

	    //manager->sendTcpPacket(handle, 1, data, length);
	    //manager->sendUdpDatagram(1, data, length);

	    hb->startHeartbeat();
	}
	else
	{
	    cout << "json parsing error!" << endl;
	}
    }

private:
    int handle;
    YNetworkManager* manager;
    YHeartbeatManager* hb;
};

int main()
{
    YNetworkManager* manager = new YNetworkManager("config2.ini");
    YHeartbeatManager* hb = new YHeartbeatManager();
    TestClient client(manager, hb);
    manager->addNetworkMessageListener(1, &client);
    if(!manager->start())
    {
	cerr << "network error!!" << endl;
    }

    cin.get();

    client.testMessage();

    cin.get();

    delete manager;

    return 0;
}
