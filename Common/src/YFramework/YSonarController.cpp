#include <yolo/YFramework/YSonarController.h>
#ifdef ROBOT
#include <wiringPi.h>
#endif


namespace yolo
{

YSonarController::YSonarController(int trigger, int echo)
{
    _trigger = trigger;
    _echo = echo;
    _sonarListener = nullptr;
    _continueThread = false;
    _distanceThread = nullptr;
}

YSonarController::YSonarController(int trigger, int echo, YSonarDistanceListener* listener)
{
    _trigger = trigger;
    _echo = echo;
    _sonarListener = listener;
    _continueThread = false;
    _distanceThread = nullptr;
}

YSonarController::~YSonarController()
{
    stop();
}

void
YSonarController::init(int timeout)
{
#ifdef ROBOT
    if(wiringPiSetup() != -1)
    {
	cout << "[YSonarController] sonar is initialized" << endl;
    }
    else
    {
	cout << "[YSonarController] sonar is not initialized" << endl;
    }

    pinMode(_trigger, OUTPUT);
    pinMode(_echo, INPUT);
    digitalWrite(_trigger, LOW);
    delay(500);

    _timeout = timeout;

    _continueThread = true;
    _distanceThread = new boost::thread(boost::bind(&YSonarController::distanceThread, this));
#endif
}

void
YSonarController::stop()
{
    if(_distanceThread != nullptr)
    {
	_continueThread = false;
	_distanceThread->join();
	delete _distanceThread;
	_distanceThread = nullptr;
    }
}

void
YSonarController::distanceThread()
{
#ifdef ROBOT
    long startTimeUsec,endTimeUsec,travelTimeUsec,timeoutstart;
    double distanceCm;
    const double factor = 58.0;

    while(_continueThread)
    {
	bool fail = false;

	delay(10);
	digitalWrite(_trigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(_trigger, LOW);

	timeoutstart = micros();

	while( digitalRead(_echo) == LOW )
	{
	    if( micros() - timeoutstart >= _timeout)
	    {
		fail = true;
		break;
	    }
	}

	if(fail) {
	    continue;
	}

	startTimeUsec = micros();

	while( digitalRead(_echo) == HIGH )
	{
	    endTimeUsec = micros();
	    if( endTimeUsec - timeoutstart >= _timeout)
	    {
		fail = true;
		break;
	    }
	}

	if(fail) {
	    continue;
	}

	travelTimeUsec = endTimeUsec - startTimeUsec;
	distanceCm = travelTimeUsec / factor;

	if(_sonarListener != nullptr)
	    _sonarListener->onReceiveDistance(distanceCm);

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
#endif
}

} // namespace yolo
