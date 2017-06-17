#include <iostream>

#include "YMessageSender.h"
#include "YMessageReceiver.h"
#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YHeartbeatManager.h>

using namespace yolo;

int main(int argc, char** argv)
{
    string defaultIni = "config2.ini";
    string configIni = defaultIni;
    string defaultJson = "msgformat.json";
    string msgformat = defaultJson;

    if(argc >= 2)
    {
	// 2nd arg is configuration file
	configIni = argv[1];
    }

    if(argc >= 3)
    {
	// 3rd arg is msgformat json file
	msgformat = argv[2];
    }

    YNetworkManager* manager = new YNetworkManager(configIni);
    YHeartbeatManager* hb = new YHeartbeatManager(manager);

    YImageSender imageSender(manager);
    YEventSender eventSender(manager);
    YCommandReceiver cmdReceiver(msgformat);
    YConfigReceiver configReceiver(msgformat);

    manager->setHeartbeatManager(hb);
    // call add listener function for the every message
    manager->addNetworkMessageListener(1, &cmdReceiver);
    manager->addNetworkMessageListener(2, &configReceiver);


    cin.get();

    return 0;
}
