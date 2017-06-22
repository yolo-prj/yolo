#ifndef _YOLO_ROBOT_MANAGER_H_
#define _YOLO_ROBOT_MANAGER_H_

#include <iostream>
#include <vector>
#include <yolo/YFramework/YCameraController.h>
#include <yolo/YFramework/YCameraImageListener.h>
#include <yolo/YFramework/YSonarController.h>
#include <yolo/YFramework/YSonarDistanceListener.h>
#include <yolo/YFramework/YServoController.h>
#include <yolo/YFramework/YMessage.h>
#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YMessageFormatParser.h>

#include "YMessageSender.h"
#include "YMessageReceiver.h"
#include "PID.h"
#include "ImageProcessor.h"

using namespace std;
using namespace yolo;
using namespace cv;


class RobotManager : public YCameraImageListener, public YSonarDistanceListener
		     , public YCommandListener, public YConfigListener
{
private:
    RobotManager(){}
public:
    RobotManager(YNetworkManager* manager, string msgFormatFile);
    ~RobotManager();

    // interface overrides (implementation)
    virtual void onReceiveImage(Mat image);
    virtual void onReceiveDistance(double distanceCm);
    virtual void onReceiveCommand(YMessage msg);
    virtual void onReceiveConfig(YMessage msg);

    void start();
    void stop();

    void receiveCommand(YMessage msg);
    void receiveConfig(YMessage config);

    // for test
    void commandLoop();

private:
    byte* convertImageToJPEG(Mat image, uint& length);

    // Robot's action
    void uturnLeft();
    void uturnRight();

private:
    YCameraController*	_camController;
    vector<int>		_param;
    vector<byte>	_buffer;

    YImageSender*	_imageSender;
    YEventSender*	_eventSender;
    YCommandReceiver*	_cmdReceiver;
    YConfigReceiver*	_configReceiver;

    YNetworkManager*	_networkManager;

    YCameraController*	_cameraController;
    YServoController*	_servoController;
    YSonarController*	_sonarController;

    YMessageFormatParser* _parser;

    int			_pan;
    int			_tilt;
    float		_offset;
    bool		_run;
    double		_distance;

    PID			_pid;
    int			_id;
};


#endif
