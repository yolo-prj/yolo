#include <iostream>
#include <yolo/YFramework/YNetworkManager.h>


using namespace std;
using namespace yolo;


int main()
{
    YNetworkManager* manager = new YNetworkManager("config.ini");
    if(!manager->start())
    {
	cerr << "network error!!" << endl;
    }


    cin.get();

    delete manager;

    return 0;
}
