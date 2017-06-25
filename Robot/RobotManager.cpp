#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <signal.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include "RobotManager.h"
#include <time.h>
#include <unistd.h>

#include "yolo/YFramework/YServoController.h"


using namespace boost;
using namespace yolo;


#define WIDTH	(640)
#define HEIGHT	(480)


#define LINE_WIDTH	(100)



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




CRobotMgr::CRobotMgr()
{
	
}

CRobotMgr::CRobotMgr(YNetworkManager* manager, string msgFormatFile)
{


	m_current_mode = RBT_UNDEFINE_MODE;
	m_current_status = RBT_STOP_ST;


	m_trackregion = Rect(10, 2*HEIGHT/3, WIDTH-20, HEIGHT/12);
	m_vision.SetCameraParam(WIDTH,HEIGHT, m_trackregion);
	m_vision.ChangeVisionMode(VISION_STOP,50);
	m_lasttrack=Rect(m_trackregion.x + (m_trackregion.width-LINE_WIDTH)/2,m_trackregion.y,m_trackregion.x + (m_trackregion.width+LINE_WIDTH)/2,m_trackregion.y+m_trackregion.height);

	m_unrecognization = 0;
	m_track_direction = RBT_STRAIGHT;

	for(int i=0; i<5; i++)
		m_average[i]=Point(0,0);
	m_averageindex = 0;



    m_image_param.resize(2);
    m_image_param[0] = cv::IMWRITE_JPEG_QUALITY;
    m_image_param[1] = 80;

    m_networkManager = manager;
    m_imageSender = new YImageSender(manager);
    m_eventSender = new YEventSender(manager);
    m_cmdReceiver = new YCommandReceiver(this, msgFormatFile);
    m_configReceiver = new YConfigReceiver(this, msgFormatFile);

    m_sonarController = new YSonarController(28, 29, this);

    m_parser = new YMessageFormatParser(msgFormatFile);
    m_parser->parse();


    _id = 0;






    // fix below for the msg interfaces
    manager->addNetworkMessageListener(1, m_cmdReceiver);
    manager->addNetworkMessageListener(2, m_cmdReceiver);
    manager->addNetworkMessageListener(3, m_cmdReceiver);
    manager->addNetworkMessageListener(4, m_cmdReceiver);
    manager->addNetworkMessageListener(5, m_cmdReceiver);
    manager->addNetworkMessageListener(6, m_cmdReceiver);
    manager->addNetworkMessageListener(7, m_cmdReceiver);
    manager->addNetworkMessageListener(8, m_cmdReceiver);
    manager->addNetworkMessageListener(9, m_cmdReceiver);
    manager->addNetworkMessageListener(10, m_cmdReceiver);
    manager->addNetworkMessageListener(11, m_cmdReceiver);
    manager->addNetworkMessageListener(12, m_cmdReceiver);
    manager->addNetworkMessageListener(13, m_cmdReceiver);
    manager->addNetworkMessageListener(14, m_cmdReceiver);
    manager->addNetworkMessageListener(21, m_cmdReceiver);
    manager->addNetworkMessageListener(31, m_cmdReceiver);
    manager->addNetworkMessageListener(300, m_cmdReceiver);

    manager->addNetworkMessageListener(200, m_configReceiver);

}


CRobotMgr::~CRobotMgr()
{

	m_servo_ctrl.stop();


	delete m_sonarController;
	delete m_parser;
	delete m_imageSender;
	delete m_eventSender;
	delete m_cmdReceiver;
	delete m_configReceiver;

	
}


void CRobotMgr::StartRobot()
{
	m_servo_ctrl.start();
	m_vision.SetInterface((CVisionInf*)this);

    m_sonarController->init(30000);

	ChangeRobotMode(RBT_NORMAL_MODE);
}


void CRobotMgr::StopRobot()
{
    m_servo_ctrl.stop();

    m_sonarController->stop();
    m_networkManager->stop();
}




bool CRobotMgr::ChangeRobotMode(E_RBT_MODE mode)
{

	if (m_current_mode ==  mode)
		return false;
		
	m_current_mode = mode;

	switch (mode)
	{
	case RBT_NORMAL_MODE:
		m_servo_ctrl.resetServo();
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
	break;
	case RBT_TRACK_MODE:
		m_servo_ctrl.setCameraDirection(TRK_LINE_CAM_PAN,TRK_LINE_CAM_TILT);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
		m_servo_ctrl.initPID();

		m_unrecognization = 0;
		m_lasttrack=Rect(m_trackregion.x + (m_trackregion.width-LINE_WIDTH)/2,m_trackregion.y,m_trackregion.x + (m_trackregion.width+LINE_WIDTH)/2,m_trackregion.y+m_trackregion.height);
		m_lastx = m_trackregion.x + m_trackregion.width/2;
		m_track_direction = RBT_STRAIGHT;
		
		for(int i=0; i<5; i++)
			m_average[i]=Point(0,0);
		m_averageindex = 0;

		m_vision.ChangeVisionMode(VISION_TRACK,10);
		
	break;
	case RBT_SUSPEND_MODE:
		m_servo_ctrl.resetServo();
		m_vision.ChangeVisionMode(VISION_STOP,30);
	break;
	case RBT_SIGN_MODE:
		m_servo_ctrl.resetServo();
		m_vision.ChangeVisionMode(VISION_SIGN,100);
	break;
	case RBT_AVOIDCAN_MODE:
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
	break;
	case RBT_PUSHCAN_MODE:
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
	break;

	default:
		m_current_mode = RBT_NORMAL_MODE;
		m_servo_ctrl.resetServo();
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
	break;
	}
}


vector<Rect> CRobotMgr::EliminateUselessPath(vector<Rect> & linelist)
{
	vector<Rect> newlist;
	int i;
	
	for(i = linelist.size()-1; i>=0; i--)
	{
		if ((m_lasttrack & linelist[i]).width > 1)
			newlist.push_back(linelist[i]);
	}

	return newlist;
}

int CRobotMgr::GetStraightLine()
{
	int xmin=INT_MAX;
	int xmax=-INT_MAX;;
	int ymin=INT_MAX;
	int ymax=-INT_MAX;;

	for (int a=0; a<5; a++)
	{
		if (m_average[a].x)
		{
			xmin = (xmin>m_average[a].x)?m_average[a].x:xmin;
			xmax = (xmax<m_average[a].x)?m_average[a].x:xmax;
		}
		if (m_average[a].y)
		{
			ymin = (ymin>m_average[a].y)?m_average[a].y:ymin;
			ymax = (ymax<m_average[a].y)?m_average[a].y:ymax;
		}

		m_average[a]=Point(0,0);
	}

	if ((xmax-xmin) < (ymax-ymin))
		return 0;


	return 1;
}


Rect CRobotMgr::SelectPath(vector<Rect> & linelist,vector<Rect> & eliminated_linelist, E_RBT_NEXT_DIRECTION direction, int *outx)
{
	Rect newregion;
	vector<Rect> * destlist;
	int newposition=m_lastx;
	int i;
	int distance=INT_MAX;
	int select=0;



	if (eliminated_linelist.size()>0)
		destlist = &eliminated_linelist;
	else
		destlist = &linelist;
	
	if ((*destlist).size()>0)
	{
		newregion = (*destlist)[0];
//		distance = abs(WIDTH/2 - ((*destlist)[0].x+(*destlist)[0].width/2));
		distance = (*destlist)[0].x;
		for(i = 1; i<destlist->size(); i++)
		{
			switch(direction)
			{
			case RBT_STRAIGHT:
				if (i==1)
				{
					select = GetStraightLine();
				}
				if ((select && distance < (*destlist)[i].x) ||(!select && distance > (*destlist)[i].x))
				{
					distance = (*destlist)[i].x;
					newregion = (*destlist)[i];
				}
				
//				if (distance > abs(WIDTH/2 - ((*destlist)[i].x+(*destlist)[i].width/2)))
//				{
//					distance = abs(WIDTH/2 - ((*destlist)[i].x+(*destlist)[i].width/2));
//					newregion = (*destlist)[i];
//				}
			break;
			case RBT_LEFT:
				if ((newregion.x+newregion.width/2) > ((*destlist)[i].x+(*destlist)[i].width/2))
				{
					newregion = (*destlist)[i];
				}
			break;
			case RBT_RIGHT:
				if ((newregion.x+newregion.width/2) < ((*destlist)[i].x+(*destlist)[i].width/2))
				{
					newregion = (*destlist)[i];
				}
			break;
			} 
		}
	}


//	if ((*destlist).size()==1)
//	{
//		int temp = newregion.x + (float)(newregion.width*2) * (float)(newposition)/(float)(WIDTH);
//		if (abs(temp-WIDTH/2) > abs(newposition-WIDTH/2))
//			newposition = temp;
//	}


//	if ((*destlist).size()==1)
//	{
		if ( (newposition-LINE_WIDTH/2) < newregion.x)
			newposition-=(newposition-LINE_WIDTH/2) - newregion.x;
		if ( (newposition+LINE_WIDTH/2) > newregion.br().x)
			newposition-=(newposition+LINE_WIDTH/2) - newregion.br().x;
//	}


	if ((*destlist).size()==1)
	{
		if ( (WIDTH/2) < newregion.x)
			newposition = newregion.br().x - LINE_WIDTH/2;
		if ( (WIDTH/2) > newregion.br().x)
			newposition =newregion.x + LINE_WIDTH/2;

		if (newregion.width > WIDTH*2/3)
		{
			if ( (WIDTH/2) < newposition)
				newposition = newregion.br().x - LINE_WIDTH/2;
			if ( (WIDTH/2) > newposition)
				newposition =newregion.x + LINE_WIDTH/2;
		}
	}


/*	else
	{
		switch(direction)
		{
		case RBT_LEFT:
			newposition=newregion.x+LINE_WIDTH/2;
		break;
		case RBT_RIGHT:
			newposition=newregion.br().x-LINE_WIDTH/2;
		break;
		default:
			if ( (newposition-LINE_WIDTH/2) < newregion.x)
				newposition-=(newposition-LINE_WIDTH/2) - newregion.x;
			if ( (newposition+LINE_WIDTH/2) > newregion.br().x)
				newposition-=(newposition+LINE_WIDTH/2) - newregion.br().x;
		break;
		}
	}
*/


	
	m_average[m_averageindex]=Point(newregion.x,newregion.x+newregion.width);
	m_averageindex = (m_averageindex+1)%5;

	*outx = newposition	;
		
	return newregion;
}








void CRobotMgr::ChangeMode_Thread(E_RBT_MODE mode)
{
	ChangeRobotMode(mode);
}


void CRobotMgr::CreateChangModeThread(E_RBT_MODE mode)
{
	thread(&CRobotMgr::ChangeMode_Thread,this,mode);
}

void CRobotMgr::Assert_TrackMode()
{
	printf("Lost Track \n");
	CreateChangModeThread(RBT_NORMAL_MODE);
}


void CRobotMgr::onVisionImage(Mat & image)
{
	uint length;
	byte* data = convertImageToJPEG(image,length);

    YMessage msg;
    msg.setOpcode(10);
    msg.set(data, length);

    m_imageSender->send(msg);
	
}



Rect CRobotMgr::onLineDetect(vector<Rect> & linelist)
{
	int dest_pt;
	double offsetfromcenter;
	Rect temprect;
	vector<Rect> templist;
	
	if(m_current_mode != RBT_TRACK_MODE)
		return Rect(0,0,0,0);

	if (linelist.size() == 0)
	{
		m_unrecognization++;
		if (m_unrecognization>3)
			Assert_TrackMode();
		return Rect(0,0,0,0);
	}


	m_unrecognization=0;

	templist = EliminateUselessPath(linelist);
	m_lasttrack = SelectPath(linelist,templist,m_track_direction,&dest_pt);
	m_lastx = dest_pt;
	
	offsetfromcenter=1.0f - 2.0f*(float)dest_pt/(float)WIDTH;
	
	offsetfromcenter = m_servo_ctrl.runPID(offsetfromcenter+0.001);


	if (offsetfromcenter>BASESPEED)
		offsetfromcenter = BASESPEED;
		
	if (offsetfromcenter<-BASESPEED)
		offsetfromcenter = -BASESPEED;
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, BASESPEED - offsetfromcenter);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, BASESPEED + offsetfromcenter);




//	cout<<"sel ("<<templist.size()<<") "<<m_lasttrack.tl()<<" "<<m_lasttrack.br()<<" "<<dest_pt<<endl;

//	for (int k=0; k<templist.size();k++)
//		cout<<k<<" "<<templist[k].tl()<<" "<<templist[k].br()<<" "<<dest_pt<<endl;


	return Rect(m_lastx-LINE_WIDTH/2, m_trackregion.y, LINE_WIDTH, m_trackregion.height);


//	return m_lasttrack;
}



Vec3f CRobotMgr::onColorCircle(vector<cv::Vec3f> & circlelist)
{
	int i;

	for (i=0; i< circlelist.size(); i++)
		cout<<"Circle "<<circlelist[i]<<endl;
}
Rect CRobotMgr::onStopBar(vector<Rect> & linelist)
{
	int i;

	for (i=0; i< linelist.size(); i++)
		cout<<"Stopbar "<<linelist[i]<<endl;
}




bool CRobotMgr::onObstacle()
{
}

bool CRobotMgr::onStopBar()
{
}

bool CRobotMgr::onSignDetect()
{
}

bool CRobotMgr::onPushCan()
{
}








bool CRobotMgr::MoveForward(int  velocity)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, BASESPEED + velocity);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, BASESPEED + velocity);

}

bool CRobotMgr::MoveBackward(int velocity)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, BASESPEED - velocity);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, BASESPEED - velocity);

}

bool CRobotMgr::TurnLeft(int velocity)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, BASESPEED - velocity);
	return m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, BASESPEED + velocity);
}
bool CRobotMgr::TurnRight(int velocity)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, BASESPEED + velocity);
	return m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, BASESPEED - velocity);
}
bool CRobotMgr::TurnAround()
{
}

bool CRobotMgr::SetCameraTilt(int degree)
{
	return m_servo_ctrl.setServoPosition(ENUM_SERVO_PAN, degree);
}
bool CRobotMgr::SetCameraPan(int degree)
{
	
	return m_servo_ctrl.setServoPosition(ENUM_SERVO_TILT, degree);
}




void CRobotMgr::onReceiveDistance(double distanceCm)
{
//    cout << "[RobotManager] receive distance : " << distanceCm << " cm" << endl;
}

void CRobotMgr::onReceiveCommand(YMessage msg)
{
	cout << "[RobotManager] received command" << endl;
	int speed = 0;

	YMessage sendMsg = m_parser->getMessage(1001);



	switch(msg.getOpcode())
	{
	case OP_RECV_MOVE_CNTR_FORWARD:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;
		if (msg.getInt("state")==1)
			MoveForward(5);
		else
			MoveForward(0);
	break;
	case OP_RECV_MOVE_CNTR_BACKWARD:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;
		if (msg.getInt("state")==1)
			MoveBackward(5);
		else
			MoveBackward(0);

	break;
	case OP_RECV_MOVE_CNTR_LEFT:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;
		if (msg.getInt("state")==1)
			TurnLeft(5);
		else
			TurnLeft(0);

	break;
	case OP_RECV_MOVE_CNTR_RIGHT:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;
		if (msg.getInt("state")==1)
			TurnRight(5);
		else
			TurnRight(0);

	break;
	case OP_RECV_MOVE_CNTR_UTERN:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;

	break;
	case 6:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;

	break;
	case 7:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;

	break;
	case 8:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;

	break;
	case 9:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;

	break;
	case 10:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;
	break;
	case OP_RECV_CAMERA_CNTR_PAN:
		cout << "[RobotManager] " << msg.getString("command") << ", angle : " << msg.getInt("state") << endl;
		SetCameraPan(msg.getInt("state"));

	break;
	case OP_RECV_CAMERA_CNTR_TILT:
		cout << "[RobotManager] " << msg.getString("command") << ", angle : " << msg.getInt("state") << endl;
		SetCameraTilt(msg.getInt("state"));

	break;
	case 13:
		cout << "[RobotManager] Start Robot!!" << endl;
		ChangeRobotMode(RBT_TRACK_MODE);
	break;
	case 14:
		cout << "[RobotManager] Stop Robot!!" << endl;
		ChangeRobotMode(RBT_NORMAL_MODE);

		sendMsg.setOpcode(1001);
		sendMsg.set("id", _id);
		sendMsg.set("event", 2);
		sendMsg.set("state", 3);
		m_eventSender->send(sendMsg);
	break;

	case OP_RECV_MODE_CHANGE:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;

		switch(msg.getInt("state"))
		{
			case 1:
				ChangeRobotMode(RBT_TRACK_MODE);
			break;
			case 2:
				ChangeRobotMode(RBT_NORMAL_MODE);
			break;
			case 3:
				ChangeRobotMode(RBT_SUSPEND_MODE);
			break;
		}

		sendMsg.setOpcode(1001);
		sendMsg.set("id", _id);
		sendMsg.set("event", "mode_changed");
		sendMsg.set("state", msg.getInt("state"));
		m_eventSender->send(sendMsg);
	break;

	case OP_RECV_IMAGE_SEND:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;
	break;


	case OP_RECV_INIT_COMMAND:
		_id = msg.getInt("id");
		cout << "[RobotManager] Robot ID : " << _id <<endl;
	break;

	case OP_RECV_DEGUG:
		cout << "[RobotManager] " << msg.getString("debug") << ", state : " << msg.getInt("state") << endl;
		sendMsg.setOpcode(1300);
		sendMsg.set("id", _id);
		sendMsg.set("debug_info", "Robot Debug Info");
		sendMsg.set("state", 0);
		m_eventSender->send(sendMsg);
	break;
	}



}

void CRobotMgr::onReceiveConfig(YMessage msg)
{
    cout << "[RobotManager] received config message" << endl;

    switch(msg.getOpcode())
    {
    case 200:
	_id = msg.getInt("id");
	cout << "[RobotManager] Robot ID : " << _id << endl;
	break;
    }
}

void CRobotMgr::connectionLost(string addr, ushort port)
{
    cout << "[RobotManager] CONNECTION LOST : " << addr << ", port: " << port << endl;
}

byte* CRobotMgr::convertImageToJPEG(Mat image, uint& length)
{
    byte* data;

    cv::imencode(".jpg", image, m_image_buffer, m_image_param);

    data = m_image_buffer.data();
    length = m_image_buffer.size();

    return data;
}

