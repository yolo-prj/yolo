#ifndef ROBOTEVENTLISTENER_H
#define ROBOTEVENTLISTENER_H
#include <yolo/YFramework/YMessage.h>
#include <tuple>
#include "robotmode.h"
#include "robotmovement.h"

namespace yolo {
class YMessage;
class RobotEventListener
{
public:
    virtual void OnRobotModeChanged(int handler, int mode) = 0;
    virtual void OnRobotErrorEventReceived(int handler, int error) = 0;
    virtual void OnRobotMoveEventReceived(int handler, int move) = 0;

    virtual void OnRobotConnected(int handler) = 0;
    virtual void OnRobotDisconnected(int handler) = 0;

    virtual void OnRobotInvalidDisconnected(int handler) = 0;
    virtual void OnRobotReconnected(int handler) = 0;

    virtual void OnRobotDebugInfoReceived(int handler, const std::string debug_info, int state) = 0;
};
}

#endif // ROBOTEVENTLISTENER_H
