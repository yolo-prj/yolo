#ifndef _YOLO_SONAR_CONTROLLER_H_
#define _YOLO_SONAR_CONTROLLER_H_

#include "YSonarDistanceListener.h"
#include <boost/thread.hpp>

using namespace std;

namespace yolo
{

class YSonarController
{
public:
    YSonarController(int trigger, int echo);
    YSonarController(int trigger, int echo, YSonarDistanceListener*);
    ~YSonarController();

    void init(int timeout);
    void stop();

    inline void setSonarDistanceListener(YSonarDistanceListener* listener) {
	_sonarListener = listener;
    }

private:
    void distanceThread();

private:
    YSonarDistanceListener* _sonarListener;
    boost::thread*	    _distanceThread;
    int			    _trigger;
    int			    _echo;
    bool		    _continueThread;
    int			    _timeout;
};

}

#endif
