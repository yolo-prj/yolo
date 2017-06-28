#include <yolo/YFramework/YServoController.h>

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>

#include <time.h>
#include <math.h>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

namespace yolo
{

YServoController::YServoController()
{
    _servoLimits.resize(NUM_SERVOS);

    for(int i=0; i<NUM_SERVOS; i++) {
		_servoLimits[i].resize(2);
    }

    _updated.resize(NUM_SERVOS);
    fill(_updated.begin(), _updated.end(), false);

    _position.resize(NUM_SERVOS);

    _servoLimits[0][0] = PAN_CAMERA_MIN;
    _servoLimits[0][1] = PAN_CAMERA_MAX;
    _servoLimits[1][0] = TILT_CAMERA_MIN;
    _servoLimits[1][1] = TILT_CAMERA_MAX;
    _servoLimits[2][0] = WHEEL_MIN;
    _servoLimits[2][1] = WHEEL_MAX;
    _servoLimits[3][0] = WHEEL_MIN;
    _servoLimits[3][1] = WHEEL_MAX;

    _panControlThread = nullptr;
    _tiltControlThread = nullptr;
    _leftControlThread = nullptr;
    _rightControlThread = nullptr;

    _servoFd = -1;
    _continueThread = false;
}

YServoController::~YServoController()
{
	stop();
}

bool
YServoController::openServos()
{
    bool success = false;

    if(_servoFd == -1)
    {
		_servoFd = open("/dev/servoblaster", O_RDWR);

		if(_servoFd >= 0)
		{
		    success = true;
		}
    }


    return success;
}

void
YServoController::closeServos()
{
    if(_servoFd != -1)
	close(_servoFd);
}

void
YServoController::start()
{
    if(!openServos())
    {
	cerr << "[YServoController] servo init failed." << endl;
	return;
    }
    else
    {
	cout << "[YServoController] servo is initialized" << endl;
    }

    resetServo();
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    /*
    _continueThread = true;
    _panControlThread = new boost::thread(boost::bind(&YServoController::panControlThread, this));
    _tiltControlThread = new boost::thread(boost::bind(&YServoController::tiltControlThread, this));
    _leftControlThread = new boost::thread(boost::bind(&YServoController::leftControlThread, this));
    _rightControlThread = new boost::thread(boost::bind(&YServoController::rightControlThread, this));
    */
}

void
YServoController::stop()
{
    _continueThread = false;
	closeServos();

    /*
    if(_panControlThread) {
	if(_panControlThread->joinable())
	    _panControlThread->join();
	delete _panControlThread;
	_panControlThread = nullptr;
    }

    if(_tiltControlThread) {
	if(_tiltControlThread->joinable())
	_tiltControlThread->join();
	delete _tiltControlThread;
	_tiltControlThread = nullptr;
    }

    if(_leftControlThread) {
	if(_leftControlThread->joinable())
	_leftControlThread->join();
	delete _leftControlThread;
	_leftControlThread = nullptr;
    }

    if(_rightControlThread) {
	if(_rightControlThread->joinable())
	    _rightControlThread->join();
	delete _rightControlThread;
	_rightControlThread = nullptr;
    }
    */
}

void
YServoController::panControlThread()
{
    while(_continueThread)
    {
	bool update = false;
	string position;
	{
	    boost::mutex::scoped_lock lock(_mutex);
	    update = _updated[CAMERA_PAN];
	    position = _position[CAMERA_PAN];
	}

	if(update)
	{
	    write(_servoFd, position.c_str(), position.length());
	    fsync(_servoFd);

	    {
		boost::mutex::scoped_lock lock(_mutex);
		_updated[CAMERA_PAN] = false;
	    }
	}
	else
	{
	    boost::this_thread::sleep(boost::posix_time::milliseconds(5));
	}
    }
}

void
YServoController::tiltControlThread()
{
    while(_continueThread)
    {
	bool update = false;
	string position;
	{
	    boost::mutex::scoped_lock lock(_mutex);
	    update = _updated[CAMERA_TILT];
	    position = _position[CAMERA_TILT];
	}

	if(update)
	{
	    write(_servoFd, position.c_str(), position.length());
	    fsync(_servoFd);

	    {
		boost::mutex::scoped_lock lock(_mutex);
		_updated[CAMERA_TILT] = false;
	    }
	}
	else
	{
	    boost::this_thread::sleep(boost::posix_time::milliseconds(5));
	}
    }
}

void
YServoController::leftControlThread()
{
    while(_continueThread)
    {
	bool update = false;
	string position;
	{
	    boost::mutex::scoped_lock lock(_mutex);
	    update = _updated[LEFT_WHEEL];
	    position = _position[LEFT_WHEEL];
	}

	if(update)
	{
	    write(_servoFd, position.c_str(), position.length());
	    fsync(_servoFd);

	    {
		boost::mutex::scoped_lock lock(_mutex);
		_updated[LEFT_WHEEL] = false;
	    }
	}
	else
	{
	    boost::this_thread::sleep(boost::posix_time::milliseconds(5));
	}
    }
}

void
YServoController::rightControlThread()
{
    while(_continueThread)
    {
	bool update = false;
	string position;
	{
	    boost::mutex::scoped_lock lock(_mutex);
	    update = _updated[RIGHT_WHEEL];
	    position = _position[RIGHT_WHEEL];
	}

	if(update)
	{
	    write(_servoFd, position.c_str(), position.length());
	    fsync(_servoFd);

	    {
		boost::mutex::scoped_lock lock(_mutex);
		_updated[RIGHT_WHEEL] = false;
	    }
	}
	else
	{
	    boost::this_thread::sleep(boost::posix_time::milliseconds(5));
	}
    }
}

bool
YServoController::setServoPosition(ServoType type, int& position)
{
    bool success = false;
    const int LIMIT_MAX = 1;
    const int LIMIT_MIN = 0;
    char buf[256];


	if (_servoFd<=0)
		return false;

	if(position > _servoLimits[type][LIMIT_MAX])
		position = _servoLimits[type][LIMIT_MAX];
	else if(position < _servoLimits[type][LIMIT_MIN])
		position = _servoLimits[type][LIMIT_MIN];

    sprintf(buf, "%d=%d\n", type, position);

    write(_servoFd, buf, strlen(buf));
    fsync(_servoFd);
    /*
    {
	boost::mutex::scoped_lock lock(_mutex);

	_position[type] = buf;
	_updated[type] = true;

	success = true;
    }
    */
	
	return true;
}



bool
YServoController::setWheelSpeed(ServoType type, int speed)
{
    if(type == ENUM_SERVO_LEFT_WHEEL)
    {
		speed += SERVO_CENTER_OR_STOP;
		return setServoPosition(type, speed);
    }
    else if(type == ENUM_SERVO_RIGHT_WHEEL)
    {
		speed = -speed + SERVO_CENTER_OR_STOP;
		return setServoPosition(type, speed);
    }

	return false;
}



long YServoController::getTimeMs(void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    return((intmax_t)s*1000+ms);
}


void YServoController::initPID() 
{
	pdi.LastErrorTime =getTimeMs();
	pdi.Kp = KP;
	pdi.Ki = KI;
	pdi.Kd = KD;

	pdi.BaseSpeed = BASESPEED;
	pdi.SpeedFudgeFactor=BASESPEEDFUDGEFACTOR;
	pdi.LastError = 0.0;
	pdi.Integral = 0.0;
}

void YServoController::initPID(double kp, double ki, double kd, double basespeed, double fudge) 
{
	pdi.LastErrorTime =getTimeMs();
	pdi.Kp = kp;
	pdi.Ki = ki;
	pdi.Kd = kd;

	pdi.BaseSpeed = basespeed;
	pdi.SpeedFudgeFactor=fudge;
	pdi.LastError = 0.0;
	pdi.Integral = 0.0;
}

double YServoController::runPID(double ErrorIn) 
{
	long iterationTime ;
	double error;
	long errortime;
	double correction;
	double derivation;


	error = (pdi.BaseSpeed / pdi.SpeedFudgeFactor) * ErrorIn;
	errortime =getTimeMs();


	iterationTime = (long) (errortime - pdi.LastErrorTime);
	pdi.Integral = (error / iterationTime) + pdi.Integral;
	derivation = (error - pdi.LastError) / iterationTime;
	correction = pdi.Kp * error + pdi.Ki * pdi.Integral + pdi.Kd * derivation;

	pdi.LastError = error;
	pdi.LastErrorTime = errortime;

	return(correction);
}




void
YServoController::resetServo()
{
    int value = SERVO_CENTER_OR_STOP;
    setServoPosition(ENUM_SERVO_RIGHT_WHEEL, value);
    setServoPosition(ENUM_SERVO_LEFT_WHEEL, value);
    setServoPosition(ENUM_SERVO_PAN, value);
    setServoPosition(ENUM_SERVO_TILT, value);
}

bool
YServoController::setCameraDirection(int pan, int tilt)
{
//    pan = TRK_LINE_CAM_PAN;
//    tilt = TRK_LINE_CAM_TILT;

    if (!setServoPosition(ENUM_SERVO_PAN, pan))
		return false;
    if (setServoPosition(ENUM_SERVO_TILT, tilt))
		return false;
}

} // namespace yolo
