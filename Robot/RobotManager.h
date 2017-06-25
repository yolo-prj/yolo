#ifndef _YOLO_ROBOT_MANAGER_H_
#define _YOLO_ROBOT_MANAGER_H_

#include <iostream>
#include <vector>

#include <yolo/YFramework/YSonarController.h>
#include <yolo/YFramework/YSonarDistanceListener.h>
#include <yolo/YFramework/YServoController.h>
#include <yolo/YFramework/YMessage.h>
#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YMessageFormatParser.h>
#include <yolo/YFramework/YConnectionLostListener.h>

#include "YMessageSender.h"
#include "YMessageReceiver.h"


#include "VisionInf.h"
#include "Vision.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace std;
using namespace yolo;
using namespace cv;



#define TRK_LINE_CAM_PAN	156
#define TRK_LINE_CAM_TILT	211


typedef enum
{
	RBT_UNDEFINE_MODE =0,
	RBT_TRACK_MODE=1,
	RBT_NORMAL_MODE ,
	RBT_SUSPEND_MODE ,
	RBT_SIGN_MODE,
	RBT_AVOIDCAN_MODE,
	RBT_PUSHCAN_MODE
}E_RBT_MODE;



typedef enum
{
	RBT_STOP_ST =0x0,
	RBT_TRACK_ST =0x10000,

	RBT_FORWARD_ST =0x01,
	RBT_BACKWARD_ST =0x02,
	RBT_TURNLEFT_ST =0x04,
	RBT_TURNRIGHT_ST =0x08,
	RBT_CAMERATILT_ST =0x100,
	RBT_CAMERAPAN_ST =0x200
}E_RBT_STAUTS;


typedef enum
{
	RBT_STRAIGHT = 0x0,
	RBT_LEFT = 0x1,
	RBT_RIGHT = 0x2,
}E_RBT_NEXT_DIRECTION;





class CRobotMgr : public CVisionInf, public YSonarDistanceListener
		     , public YCommandListener, public YConfigListener, public YConnectionLostListener
{
private:
    CRobotMgr();
public:
    CRobotMgr(YNetworkManager* manager, string msgFormatFile);
    ~CRobotMgr();


    void StartRobot();
    void StopRobot();

	bool ChangeRobotMode(E_RBT_MODE mode);


	int GetStraightLine();
	vector<Rect> EliminateUselessPath(vector<Rect> & linelist);
	Rect SelectPath(vector<Rect> & linelist,vector<Rect> & eliminated_linelist, E_RBT_NEXT_DIRECTION direction, int *outx);


	void ChangeMode_Thread(E_RBT_MODE mode);
	void CreateChangModeThread(E_RBT_MODE mode);

	void Assert_TrackMode();




	virtual void onVisionImage(Mat & image);
// autonomouse 
	virtual Rect onLineDetect(vector<Rect> & linelist);
	virtual Vec3f onColorCircle(vector<cv::Vec3f> & linelist);
	virtual Rect onStopBar(vector<Rect> & linelist);

	
	bool onObstacle();
	bool onStopBar();
	bool onSignDetect();
	bool onPushCan();




// manual mode command
	bool MoveForward(int velocity);
	bool MoveBackward(int velocity);
	bool TurnLeft(int velocity);
	bool TurnRight(int velocity);
	bool TurnAround();
	
	bool SetCameraTilt(int degree);
	bool SetCameraPan(int degree);



    // interface overrides (implementation)
    virtual void onReceiveDistance(double distanceCm);
    virtual void onReceiveCommand(YMessage msg);
    virtual void onReceiveConfig(YMessage msg);
    virtual void connectionLost(string addr, ushort port);


private:
    byte* convertImageToJPEG(Mat image, uint& length);



public:
	CVision m_vision;

	static CRobotMgr * m_single;
	E_RBT_NEXT_DIRECTION m_track_direction;
	

private:

	E_RBT_MODE m_current_mode;
	E_RBT_STAUTS m_current_status;
	
	YServoController m_servo_ctrl;


// track recognization
	Rect m_lasttrack;
	int  m_lastx;
	Rect m_trackregion;
	int m_unrecognization;
	Point m_average[5];
	int m_averageindex;
///////

    vector<int>		m_image_param;
    vector<byte>	m_image_buffer;

    YImageSender*	m_imageSender;
    YEventSender*	m_eventSender;
    YCommandReceiver*	m_cmdReceiver;
    YConfigReceiver*	m_configReceiver;

    YNetworkManager*	m_networkManager;

    YSonarController*	m_sonarController;

    YMessageFormatParser* m_parser;

    int			_id;
};


#endif
