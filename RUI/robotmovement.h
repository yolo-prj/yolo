#ifndef ROBOTMOVEMENT_H
#define ROBOTMOVEMENT_H
namespace yolo {
enum class RobotMovement {
    MOVE_STRAIGHT = 0,
    MOVE_TURNRIGHT = 1,
    MOVE_TURNLEFT = 2,
    MOVE_UTURN = 3,
    MOVE_AVOID = 4,
    MOVE_PUSHCAN = 5,
    MOVE_STOP = 6,
    MOVE_GO = 7
};
}

#endif // ROBOTMOVEMENT_H
