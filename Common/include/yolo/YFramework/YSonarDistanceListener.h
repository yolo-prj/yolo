#ifndef _YOLO_SONAR_DISTANCE_LISTENER_H_
#define _YOLO_SONAR_DISTANCE_LISTENER_H_


namespace yolo
{

class YSonarDistanceListener
{
public:
    virtual void onReceiveDistance(double distanceCm) = 0;
};

}


#endif
