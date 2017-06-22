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
    RobotMode mode() const { return mode_; }

    void ChangeMode(RobotMode mode) {
        mode_ = mode;
    }

private:
    int handle_;
    std::string addr_;
    unsigned int port_;
    RobotMode mode_;
};
}


#endif // ROBOTCONTROLLER_H
