#include "ruicontrol.h"
#include <QApplication>
#include "imagereceiver.h"
#include "robotcontrolmanager.h"

using namespace std;
using namespace yolo;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller w;
    RobotControlManager::GetInstance().Initialize("config2.ini", "msgformat.json");
    w.show();

    RobotControlManager::GetInstance().Start();

    return a.exec();
}
