#include <iostream>
#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YNetworkMessageListener.h>


using namespace std;
using namespace yolo;

class TestListener : public YNetworkMessageListener
{
    virtual void onReceiveMessage(byte* data, uint length) {
	cout << "message received" << endl;

	string str = (char*)(data + 8);
	cout << str << endl;


	//delete[] data;
    }

    virtual void onTcpClientConnected(int handle, string addr, ushort port) {
	cout << "tcp client connected : " << handle << ", "
	    << addr << ", " << port << endl;;
    }

    virtual void onTcpClientDisconnected(int handle, string addr, ushort port) {
	cout << "tcp client disconnected : " << handle << ", "
	    << addr << ", " << port << endl;;
    }
};

int main()
{
    YNetworkManager* manager = new YNetworkManager("config.ini");
    TestListener listener;
    manager->addNetworkMessageListener(1, &listener);

    if(!manager->start())
    {
	cerr << "network error!!" << endl;
    }


    cin.get();

    delete manager;

    return 0;
}
