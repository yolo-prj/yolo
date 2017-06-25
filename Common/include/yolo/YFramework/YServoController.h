#ifndef _YOLO_SERVO_CONTROLLER_H_
#define _YOLO_SERVO_CONTROLLER_H_

#include <vector>
#include <boost/thread.hpp>

#define SERVO_CENTER_OR_STOP	150

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



#define KP        1.0               // defaut 1.0
#define KI        0.1               // defaut 0.1
#define KD        0.0               // defaut 0.0
#define BASESPEED 6.0               // Range 0-50
#define BASESPEEDFUDGEFACTOR 0.80   // default 1.25



using namespace std;

namespace yolo
{
enum ServoType {
	ENUM_SERVO_PAN = CAMERA_PAN,
	ENUM_SERVO_TILT,
	ENUM_SERVO_RIGHT_WHEEL,
	ENUM_SERVO_LEFT_WHEEL
};

typedef struct
{
	double Kp ;			 // proportional gain
	double Ki ;			 // integral gain
	double Kd ;			 // derivative gain

	double BaseSpeed ; 	 // base speed of the robot used to compute magnigfy the error based on the speed of the robot
	double LastError;		 // previous error in the setpoint,
	long	LastErrorTime;	 // previous time of the previous error in the setpoint

	double Integral;		 // computed integral;
	double SpeedFudgeFactor;// fudge factor correction used in conjuction with the robot base speed
} TPID;

class YServoController
{
public:
    YServoController();
    ~YServoController();


    void resetServo();

    bool setWheelSpeed(ServoType type, int speed);
    bool setCameraDirection(int pan, int tilt);

    void start();
    void stop();


	long getTimeMs(void);
	void initPID() ;
	double runPID(double ErrorIn) ;

	bool setServoPosition(ServoType type, int& position);

private:

    void panControlThread();
    void tiltControlThread();
    void leftControlThread();
    void rightControlThread();


	bool openServos();
	void closeServos();


private:
    int			    _servoFd;
    TPID pdi;

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

