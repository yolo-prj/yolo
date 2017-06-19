#include <iostream>

#include "RobotManager.h"
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
    manager->setHeartbeatManager(hb);

    RobotManager robot(manager, msgformat);

    manager->start();

    cout << "Press any key to start robot, and then press any key to stop." << endl;
    cin.get();

    robot.start();

    robot.commandLoop();

    robot.stop();


    cin.get();

    return 0;
}
