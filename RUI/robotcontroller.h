#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H

#include <string>
#include "robotmode.h"

namespace yolo {
class RobotController
{
public:
    RobotController();
    RobotController(int handle, std::string addr, unsigned int port);

    int handle() const { return handle_; }
    const std::string& addr() const { return addr_; }
    unsigned int port() const { return port_; }
    RobotMode state() const { return state_; }

    void ChangeState(int state) {
        state_ = static_cast<RobotMode>(state);
    }

private:
    int handle_;
    std::string addr_;
    unsigned int port_;
    RobotMode state_;
};
}


#endif // ROBOTCONTROLLER_H
