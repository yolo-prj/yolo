#ifndef _YOLO_SERVO_CONTROLLER_H_
#define _YOLO_SERVO_CONTROLLER_H_

#include <vector>
#include <boost/thread.hpp>

#define SERVO_CENTER_OR_STOP	150

#define TRK_LINE_CAM_PAN	149
#define TRK_LINE_CAM_TILT	211

#define CAMERA_PAN		0
#define CAMERA_TILT		1
#define RIGHT_WHEEL		2
#define LEFT_WHEEL		3
#define NUM_SERVOS		4

#define PAN_CAMERA_MIN		50
#define PAN_CAMERA_MAX		250
#define TILT_CAMERA_MIN		80
#define TILT_CAMERA_MAX		220
#define WHEEL_MAX		200
#define WHEEL_MIN		100

using namespace std;

namespace yolo
{

class YServoController
{
public:
    YServoController();
    ~YServoController();

    enum ServoType {
	ENUM_SERVO_PAN = CAMERA_PAN,
	ENUM_SERVO_TILT,
	ENUM_SERVO_RIGHT_WHEEL,
	ENUM_SERVO_LEFT_WHEEL
    };

    bool openServos();
    void closeServos();

    void setServoPosition(ServoType type, int& position);
    void setWheelSpeed(ServoType type, int speed);

    void resetServo();

    void setCameraServosLineTrackMode(int& pan, int& tilt);

    void start();
    void stop();
private:

    void panControlThread();
    void tiltControlThread();
    void leftControlThread();
    void rightControlThread();


private:
    int			    _servoFd;
    vector< vector<int> >   _servoLimits; 
    vector<bool>	    _updated;
    vector<string>	    _position;

    boost::thread*	    _panControlThread;
    boost::thread*	    _tiltControlThread;
    boost::thread*	    _leftControlThread;
    boost::thread*	    _rightControlThread;

    boost::mutex	    _mutex;

    bool		    _continueThread;
};

}


#endif

