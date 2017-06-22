#include "robotcontroller.h"


namespace yolo {


RobotController::RobotController() :
    handle_(0),
    addr_(""),
    port_(0)

{

}

RobotController::RobotController(int handle, std::string addr, unsigned int port) :
    handle_(handle),
    addr_(std::move(addr)),
    port_(port),
    mode_(RobotMode::IDLE_MODE)
{

}

}
