#include <stdio.h>

#include <time.h>
#include <unistd.h>


#include "Vision.h"
#include "RobotManager.h"

#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YHeartbeatManager.h>

using namespace yolo;
using namespace std;
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
    CRobotMgr robot(manager, msgformat);



    manager->setHeartbeatManager(hb);
    hb->setConnectionLostListener(&robot);

	manager->start();
	hb->startHeartbeat();
	robot.StartRobot();


	while(1);

	hb->stopHeartbeat();
	robot.StopRobot();


    delete hb;
    delete manager;

    return 0;
}
