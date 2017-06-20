#include "RobotManager.h"

#define KP        1.0               // defaut 1.0
#define KI        0.1               // defaut 0.1
#define KD        0.0               // defaut 0.0
#define BASESPEED 6.0               // Range 0-50
#define BASESPEEDFUDGEFACTOR 0.80   // default 1.25

#define MIN_WHEEL_SPEED -50
#define MAX_WHEEL_SPEED  50


RobotManager::RobotManager(YNetworkManager* manager, string msgFormatFile)
{
    _param.resize(2);
    _param[0] = cv::IMWRITE_JPEG_QUALITY;
    _param[1] = 80;

    _networkManager = manager;
    _imageSender = new YImageSender(manager);
    _eventSender = new YEventSender(manager);
    _cmdReceiver = new YCommandReceiver(this, msgFormatFile);
    _configReceiver = new YConfigReceiver(this, msgFormatFile);

    _cameraController = new YCameraController(this);
    _sonarController = new YSonarController(28, 29, this);
    _servoController = new YServoController();

    _pan = 0;
    _tilt = 0;

    _run = false;

    // fix below for the msg interfaces
    manager->addNetworkMessageListener(1, _cmdReceiver);
    manager->addNetworkMessageListener(2, _configReceiver);
}

RobotManager::~RobotManager()
{
    delete _imageSender;
    delete _eventSender;
    delete _cmdReceiver;
    delete _configReceiver;
}

void
RobotManager::start()
{
    _pid.initPID(KP, KI, KD, BASESPEED, BASESPEEDFUDGEFACTOR);

    _cameraController->start();
    _servoController->start();
    _sonarController->init(30000);

    _servoController->setCameraServosLineTrackMode(_pan, _tilt);

}

void
RobotManager::stop()
{
    _cameraController->stop();
    _servoController->stop();
    _sonarController->stop();
    _networkManager->stop();
}

void
RobotManager::onReceiveImage(Mat image)
{
    // image processing
    _offset = ImageProcessor::findLineInImageAndComputeOffset(image);
    
    _pid.setError(_offset);

    if(_run)
    {
	const double MIN_DISTANCE = 10.0;

	double correction, left, right;
	correction = _pid.runPID();
	left = BASESPEED - correction;
	right = BASESPEED + correction;

	if(_distance > MIN_DISTANCE)
	{
	    _servoController->setWheelSpeed(YServoController::ENUM_SERVO_LEFT_WHEEL, left);
	    _servoController->setWheelSpeed(YServoController::ENUM_SERVO_RIGHT_WHEEL, right);
	}
	else
	{
	    _servoController->setWheelSpeed(YServoController::ENUM_SERVO_LEFT_WHEEL, 0);
	    _servoController->setWheelSpeed(YServoController::ENUM_SERVO_RIGHT_WHEEL, 0);
	}
    }


    uint length = 0;
    byte* data = nullptr;

    data = convertImageToJPEG(image, length);

    // pass image data to image sender object
    YMessage msg;
    msg.setOpcode(10);
    msg.set(data, length);

    _imageSender->send(msg);
}

void
RobotManager::onReceiveDistance(double distanceCm)
{
//    cout << "[RobotManager] receive distance : " << distanceCm << " cm" << endl;
    _distance = distanceCm;
}

void
RobotManager::onReceiveCommand(YMessage msg)
{
    cout << "[RobotManager] received command" << endl;
}

void
RobotManager::onReceiveConfig(YMessage msg)
{
    cout << "[RobotManager] received config message" << endl;
}

byte*
RobotManager::convertImageToJPEG(Mat image, uint& length)
{
    byte* data;

    cv::imencode(".jpg", image, _buffer, _param);

    data = _buffer.data();
    length = _buffer.size();

    return data;
}

void
RobotManager::commandLoop()
{
    char c;
    static int speed = 0;

    cout << "Press command key (r j l i m k q e w s) & press enter" << endl;
    while( (c = cin.get()) != 'x')
    {
	switch(c)
	{
	    case 'r':
		_run = true;
		break;
	    case 'j':
		_pan++;
		_servoController->setServoPosition(YServoController::ENUM_SERVO_PAN, _pan);
		break;
	    case 'l':
		_pan--;
		_servoController->setServoPosition(YServoController::ENUM_SERVO_PAN, _pan);
		break;
	    case 'i':
		_tilt--;
		_servoController->setServoPosition(YServoController::ENUM_SERVO_TILT, _tilt);
		break;
	    case 'm':
		_tilt++;
		_servoController->setServoPosition(YServoController::ENUM_SERVO_TILT, _tilt);
		break;
	    case 'k':
		_pan = SERVO_CENTER_OR_STOP;
		_tilt = SERVO_CENTER_OR_STOP;
		_servoController->setServoPosition(YServoController::ENUM_SERVO_PAN, _pan);
		_servoController->setServoPosition(YServoController::ENUM_SERVO_TILT, _tilt);
		break;
	    case 'q':
		speed--;
		if(speed < MIN_WHEEL_SPEED)
		    speed = MIN_WHEEL_SPEED;
		_servoController->setWheelSpeed(YServoController::ENUM_SERVO_RIGHT_WHEEL, speed);
		_servoController->setWheelSpeed(YServoController::ENUM_SERVO_LEFT_WHEEL, speed);
		break;
	    case 'e':
		speed++;
		if(speed > MAX_WHEEL_SPEED)
		    speed = MAX_WHEEL_SPEED;
		_servoController->setWheelSpeed(YServoController::ENUM_SERVO_RIGHT_WHEEL, speed);
		_servoController->setWheelSpeed(YServoController::ENUM_SERVO_LEFT_WHEEL, speed);
		break;
	    case 'w':
		speed = 0;
		_servoController->setWheelSpeed(YServoController::ENUM_SERVO_LEFT_WHEEL, speed);
		_servoController->setWheelSpeed(YServoController::ENUM_SERVO_RIGHT_WHEEL, speed);
		break;
	    case 's':
		_run = false;
		speed = 0;
		_servoController->setWheelSpeed(YServoController::ENUM_SERVO_LEFT_WHEEL, speed);
		_servoController->setWheelSpeed(YServoController::ENUM_SERVO_RIGHT_WHEEL, speed);
		break;
	    case 'u':
		uturnLeft();
		break;
	    case 'U':
		uturnRight();
		break;
	}

	cout << "Pan=" << _pan << ", Tilt=" << _tilt << ", Speed=" << speed << endl;
    }
}

void
RobotManager::uturnLeft()
{
    int leftSpeed = -6;
    int rightSpeed = 6;
    _servoController->setWheelSpeed(YServoController::ENUM_SERVO_RIGHT_WHEEL, rightSpeed);
    _servoController->setWheelSpeed(YServoController::ENUM_SERVO_LEFT_WHEEL, leftSpeed);
}

void
RobotManager::uturnRight()
{
    int leftSpeed = 6;
    int rightSpeed = -6;
    _servoController->setWheelSpeed(YServoController::ENUM_SERVO_RIGHT_WHEEL, rightSpeed);
    _servoController->setWheelSpeed(YServoController::ENUM_SERVO_LEFT_WHEEL, leftSpeed);
}
