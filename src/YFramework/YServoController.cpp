#include <yolo/YFramework/YServoController.h>

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>


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
	    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}
    }


    return success;
}

void
YServoController::closeServos()
{
    stop();

    if(_servoFd != -1)
	close(_servoFd);
}

void
YServoController::start()
{
    _continueThread = true;
    _panControlThread = new boost::thread(boost::bind(&YServoController::panControlThread, this));
    _tiltControlThread = new boost::thread(boost::bind(&YServoController::tiltControlThread, this));
    _leftControlThread = new boost::thread(boost::bind(&YServoController::leftControlThread, this));
    _rightControlThread = new boost::thread(boost::bind(&YServoController::rightControlThread, this));
}

void
YServoController::stop()
{
    _continueThread = false;

    if(_panControlThread) {
	_panControlThread->join();
	delete _panControlThread;
	_panControlThread = nullptr;
    }

    if(_tiltControlThread) {
	_tiltControlThread->join();
	delete _tiltControlThread;
	_tiltControlThread = nullptr;
    }

    if(_leftControlThread) {
	_leftControlThread->join();
	delete _leftControlThread;
	_leftControlThread = nullptr;
    }

    if(_rightControlThread) {
	_rightControlThread->join();
	delete _rightControlThread;
	_rightControlThread = nullptr;
    }
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
	    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
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
	    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
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
	    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
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
	    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
    }
}

void
YServoController::setServoPosition(ServoType type, int& position)
{
    bool success = false;
    const int LIMIT_MAX = 1;
    const int LIMIT_MIN = 0;
    char buf[256];

    if(position > _servoLimits[type][LIMIT_MAX])
	position = _servoLimits[type][LIMIT_MAX];
    else if(position < _servoLimits[type][LIMIT_MIN])
	position = _servoLimits[type][LIMIT_MIN];

    sprintf(buf, "%d=%d\n", type, position);

    {
	boost::mutex::scoped_lock lock(_mutex);

	_position[type] = buf;
	_updated[type] = true;

	success = true;
    }
}

void
YServoController::setWheelSpeed(ServoType type, int speed)
{
    if(type == ENUM_SERVO_LEFT_WHEEL)
    {
	speed += SERVO_CENTER_OR_STOP;
	setServoPosition(type, speed);
    }
    else if(type == ENUM_SERVO_RIGHT_WHEEL)
    {
	speed = -speed + SERVO_CENTER_OR_STOP;
	setServoPosition(type, speed);
    }
    else
    {
    }
}

void
YServoController::resetServo(ServoType type)
{
    int value = SERVO_CENTER_OR_STOP;
    setServoPosition(ENUM_SERVO_RIGHT_WHEEL, value);
    setServoPosition(ENUM_SERVO_LEFT_WHEEL, value);
    setServoPosition(ENUM_SERVO_PAN, value);
    setServoPosition(ENUM_SERVO_TILT, value);
}

void
YServoController::setCameraServo(int& pan, int& tilt)
{
    pan = TRK_LINE_CAM_PAN;
    tilt = TRK_LINE_CAM_TILT;

    setServoPosition(ENUM_SERVO_PAN, pan);
    setServoPosition(ENUM_SERVO_TILT, tilt);
}

} // namespace yolo
