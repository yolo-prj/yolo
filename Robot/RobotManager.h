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

#include "SignHandler.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace std;
using namespace yolo;
using namespace cv;






#define RBT_CAM_WIDTH	(640)
#define RBT_CAM_HEIGHT	(480)


#define LINE_WIDTH	(120)



#define OP_RECV_MOVE_CNTR_FORWARD   1
#define OP_RECV_MOVE_CNTR_BACKWARD  2
#define OP_RECV_MOVE_CNTR_LEFT      3
#define OP_RECV_MOVE_CNTR_RIGHT     4
#define OP_RECV_MOVE_CNTR_UTERN	    5

#define OP_RECV_CAMERA_CNTR_PAN	    11
#define OP_RECV_CAMERA_CNTR_TILT	12

#define OP_RECV_MODE_CHANGE         21
#define OP_RECV_IMAGE_SEND          31

#define OP_RECV_DEGUG               300
#define OP_RECV_INIT_COMMAND        200

#define OP_SEND_MODE                1001
#define OP_SEND_ERROR               1002
#define OP_SEND_DEBUG               1300


#define RBT_SIGN_RIGHT_PAN			(110)
#define RBT_SIGN_LEFT_PAN			(120)

#define RBT_SIGN_LEFT_TILT			(SERVO_CENTER_OR_STOP-2)


#if 1

#define RBT_AUTO_SPEED				(10)
#define TRK_LINE_CAM_PAN	156
#define TRK_LINE_CAM_TILT	205

#else

#define RBT_AUTO_SPEED				(6)
#define TRK_LINE_CAM_PAN	156
#define TRK_LINE_CAM_TILT	211
#endif




typedef enum
{
	RBT_UNDEFINE_MODE =0,
	RBT_TRACK_MODE=1,
	RBT_NORMAL_MODE ,
	RBT_SUSPEND_MODE ,
}E_RBT_MODE;


typedef enum
{
	RBT_TRACK_AUTO,
	RBT_TRACK_SIGN,
	RBT_TRACK_AVOIDCAN,
	RBT_TRACK_PUSHCAN,
	RBT_TRACK_TURN
}E_RBT_TRACK_SUB;


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



typedef enum
{
	RBT_MAN_FORWARD = 0x0,
	RBT_MAN_BACKWARD = 0x1,
	RBT_MAN_LEFT = 0x2,
	RBT_MAN_RIGHT = 0x3,
	RBT_MAN_TURN = 0x4
}E_RBT_MANUAL_MOVE;


class CRobotMgr : public CVisionInf, public YSonarDistanceListener
		     , public YCommandListener, public YConfigListener, public YConnectionLostListener, public CSignHandler
{
private:
    CRobotMgr();
public:
    CRobotMgr(YNetworkManager* manager, string msgFormatFile);
    ~CRobotMgr();


    void StartRobot();
    void StopRobot();

	bool ChangeRobotMode(E_RBT_MODE mode);
	bool ChangeRobotTrackMode(E_RBT_TRACK_SUB mode);


	int GetStraightLine();
	vector<Rect> EliminateUselessPath(vector<Rect> & linelist);
	Rect SelectPath(vector<Rect> & linelist,vector<Rect> & eliminated_linelist, E_RBT_NEXT_DIRECTION direction, int *outx);


	void ChangeMode_Thread(E_RBT_MODE mode);
	void CreateChangModeThread(E_RBT_MODE mode);
	void TrackMode_Thread(E_RBT_TRACK_SUB mode);
	void CreateTrackModeThread(E_RBT_TRACK_SUB mode);

	void Assert_TrackMode(int error);
	bool AvoidCanThread(int velocity);
	bool PushCanThread(int velocity);
	bool TurnAroundThread(int velocity);
	bool AutoTurnAroundThread(int velocity);
	long getPassTime(timespec StartTime, timespec CurrentTime);
	bool TurnCircular(int velocity, int r);







// manual mode command
	bool MoveForward(int velocity);
	bool MoveBackward(int velocity);
	bool TurnLeft(int velocity);
	bool TurnRight(int velocity);
	
	bool SetCameraTilt(int degree);
	bool SetCameraPan(int degree);

	bool ManualMoveThread(E_RBT_MANUAL_MOVE move);
	bool CreateManualMove(E_RBT_MANUAL_MOVE move);
	bool StopManualMove();



	void DebugPrint(string message);
	void SendModeChangeEvt(int type);
	void SendMoveEvt(int type);

	
	
	// callback functions (CVision interface overrides )
	virtual void onVisionImage(Mat & image); 
	virtual Rect onLineDetect(vector<Rect> & linelist);
	virtual Vec3f onColorCircle(vector<cv::Vec3f> & linelist);
	virtual Rect onStopBar(vector<Rect> & linelist);
	virtual void onSignDetect(bool isdetected);
	
	// callback function (CSignHandler interface overrides )
	virtual void onSignAction(int id, string action);
	


    // callback function (Servo, Event, Network  interface overrides )
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

	vector<CSignImage*> m_signlist;
	double m_last_sonar;

	timespec m_prev_time;
	timespec m_sign_time;


private:

	E_RBT_MODE m_current_mode;
	E_RBT_TRACK_SUB m_track_sub;
	E_RBT_STAUTS m_current_status;
	
	YServoController m_servo_ctrl;
	thread m_automode_thread;;

	thread m_manual_thread;;

	bool m_bImagesend;
	
	int m_signfailcount;

	
// track recognization
	Rect m_lasttrack;
	int  m_lastx;
	Rect m_trackregion;
	int m_unrecognization;
	Point m_average[5];
	int m_averageindex;
	int m_bRedbarDiscard;
///////
	bool m_bRedBar;
	Rect m_redbar;
	int m_redcount;



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
