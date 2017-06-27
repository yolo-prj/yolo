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


#define RBT_SIGN_RIGHT_PAN			(120)
#define RBT_SIGN_LEFT_PAN			(180)

CRobotMgr::CRobotMgr()
{
	
}

CRobotMgr::CRobotMgr(YNetworkManager* manager, string msgFormatFile)
{

	m_current_mode = RBT_UNDEFINE_MODE;
	m_track_sub = RBT_TRACK_AUTO;
	m_current_status = RBT_STOP_ST;


	m_trackregion = Rect(10, 2*HEIGHT/3, WIDTH-20, HEIGHT/12);
	m_vision.SetCameraParam(WIDTH,HEIGHT, m_trackregion);
	m_vision.ChangeVisionMode(VISION_STOP,50);
	m_lasttrack=Rect(m_trackregion.x + (m_trackregion.width-LINE_WIDTH),m_trackregion.y,LINE_WIDTH*2,m_trackregion.height);

	m_unrecognization = 0;
	m_track_direction = RBT_STRAIGHT;

	for(int i=0; i<5; i++)
		m_average[i]=Point(0,0);
	m_averageindex = 0;


	clock_gettime(CLOCK_REALTIME, &m_prev_time);

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


	m_bImagesend = true;
	
	m_bRedbarDiscard=false;

    // fix below for the msg interfaces
    manager->addNetworkMessageListener(OP_RECV_MOVE_CNTR_FORWARD, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_MOVE_CNTR_BACKWARD, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_MOVE_CNTR_LEFT, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_MOVE_CNTR_RIGHT, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_MOVE_CNTR_UTERN, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_CAMERA_CNTR_PAN, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_CAMERA_CNTR_TILT, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_MODE_CHANGE, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_IMAGE_SEND, m_cmdReceiver);
    manager->addNetworkMessageListener(OP_RECV_DEGUG, m_cmdReceiver);

    manager->addNetworkMessageListener(OP_RECV_INIT_COMMAND, m_configReceiver);


	m_signlist.push_back(new CSignImage(0,"sign/arrowS.jpg","STRAIGHT",this));
	m_signlist.push_back(new CSignImage(1,"sign/arrowL.jpg","LEFT",this));
	m_signlist.push_back(new CSignImage(2,"sign/arrowR.jpg","RIGHT",this));
	m_signlist.push_back(new CSignImage(3,"sign/arrowT.jpg","TURN",this));
	m_signlist.push_back(new CSignImage(4,"sign/arrowPush.jpg","PUSH",this));
	m_signlist.push_back(new CSignImage(5,"sign/arrowStop.jpg","STOP",this));

}


CRobotMgr::~CRobotMgr()
{

	m_servo_ctrl.stop();


	delete m_parser;
	delete m_sonarController;
	delete m_configReceiver;
	delete m_cmdReceiver;
	delete m_eventSender;
	delete m_imageSender;

	for (int i=0; i<m_signlist.size();i++)
		delete m_signlist[i];

	
}


void CRobotMgr::StartRobot()
{
	m_servo_ctrl.start();
	m_vision.SetInterface((CVisionInf*)this, &m_signlist);


	m_last_sonar=0;
    m_sonarController->init(30000);

	ChangeRobotMode(RBT_NORMAL_MODE);
}


void CRobotMgr::StopRobot()
{
    m_servo_ctrl.stop();
	m_last_sonar=0;
    m_sonarController->stop();
    m_networkManager->stop();
}




bool CRobotMgr::ChangeRobotMode(E_RBT_MODE mode)
{

	if (m_current_mode ==  mode)
		return false;
		


	if (m_automode_thread.get_id() !=  thread::id())
	{
		m_automode_thread.interrupt();
		cout<<"join"<<endl;
		m_automode_thread.join();
	}


	m_vision.Stop_Vision();

	switch (mode)
	{
	case RBT_NORMAL_MODE:
		m_servo_ctrl.resetServo();
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
		DebugPrint("<<Normal Mode>>");
	break;
	case RBT_TRACK_MODE:
 		
		m_servo_ctrl.setCameraDirection(TRK_LINE_CAM_PAN,TRK_LINE_CAM_TILT);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
		this_thread::sleep(posix_time::millisec(600));
		
		m_servo_ctrl.initPID();

		m_unrecognization = 0;
		m_bRedbarDiscard=false;


		m_lasttrack=Rect(m_trackregion.x + (m_trackregion.width-LINE_WIDTH),m_trackregion.y,LINE_WIDTH*2,m_trackregion.height);
		m_lastx = m_trackregion.x + m_trackregion.width/2;

		m_track_sub = RBT_TRACK_AUTO;
		m_track_direction = RBT_STRAIGHT;
		
		for(int i=0; i<5; i++)
			m_average[i]=Point(0,0);
		m_averageindex = 0;

		m_vision.ChangeVisionMode(VISION_TRACK,10);
		
		DebugPrint("<<Autonomous Mode>>");
		DebugPrint(string(": Robot Movement : STRAIGHT"));
		
	break;
	case RBT_SUSPEND_MODE:
		m_servo_ctrl.resetServo();
		m_vision.ChangeVisionMode(VISION_STOP,30);
		DebugPrint("<<Suspend Mode>>");
	break;

	default:
		m_current_mode = RBT_NORMAL_MODE;
		m_servo_ctrl.resetServo();
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
	break;
	}

	m_current_mode = mode;
	
}


bool CRobotMgr::ChangeRobotTrackMode(E_RBT_TRACK_SUB mode)
{
	if (mode==m_track_sub)
		return false;

	if (m_current_mode !=  RBT_TRACK_MODE)
	{
		cout << " Robot Mode is not autonomouse !! "<<endl;
		return false;
	}

	if (m_automode_thread.get_id() !=  thread::id())
	{
		m_automode_thread.interrupt();
		cout<<"join ChangeRobotTrackMode"<<endl;
		m_automode_thread.join();
	}

	m_track_sub = mode;
	switch (mode)
	{
	case RBT_TRACK_AUTO:
		m_servo_ctrl.setCameraDirection(TRK_LINE_CAM_PAN,TRK_LINE_CAM_TILT);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
		this_thread::sleep(posix_time::millisec(500));

		m_servo_ctrl.initPID();

		m_unrecognization = 0;
//		m_lasttrack=Rect(m_trackregion.x + (m_trackregion.width-LINE_WIDTH),m_trackregion.y,LINE_WIDTH*2,m_trackregion.height);
//		m_lastx = m_trackregion.x + m_trackregion.width/2;
		
		for(int i=0; i<5; i++)
			m_average[i]=Point(0,0);
		m_averageindex = 0;
		m_vision.ChangeVisionMode(VISION_TRACK,10);
		if (m_track_direction == RBT_STRAIGHT) 		DebugPrint(string(": Robot Movement : STRAIGHT"));
		if (m_track_direction == RBT_LEFT) 		DebugPrint(string(": Robot Movement : LEFT"));
		if (m_track_direction == RBT_RIGHT) 		DebugPrint(string(": Robot Movement : RIGHT"));
	break;
	case RBT_TRACK_SIGN:
		m_servo_ctrl.resetServo();
		m_servo_ctrl.setCameraDirection(RBT_SIGN_RIGHT_PAN,SERVO_CENTER_OR_STOP);
		
		m_vision.ChangeVisionMode(VISION_SIGN,30);
		m_signfailcount=0;
		
//		m_bRedbarDiscard=false;
		clock_gettime(CLOCK_REALTIME, &m_sign_time);
		DebugPrint(string(": Robot Movement : ROAD SIGN"));
		
	break;
	case RBT_TRACK_AVOIDCAN:
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
		m_servo_ctrl.setCameraDirection(TRK_LINE_CAM_PAN,TRK_LINE_CAM_TILT);
		m_lasttrack=Rect(m_trackregion.x ,m_trackregion.y, LINE_WIDTH*2, m_trackregion.height);
		m_lastx = m_trackregion.x + m_lasttrack.width/2;
		m_automode_thread = thread(&CRobotMgr::AvoidCanThread, this,6);
		DebugPrint(string(": Robot Movement : AVOIDING CAN"));

	break;
	case RBT_TRACK_PUSHCAN:
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
		m_servo_ctrl.setCameraDirection(TRK_LINE_CAM_PAN,TRK_LINE_CAM_TILT);
		m_lasttrack=Rect(m_trackregion.x ,m_trackregion.y, LINE_WIDTH*2, m_trackregion.height);
		m_lastx = m_trackregion.x + m_lasttrack.width/2;
		m_automode_thread = thread(&CRobotMgr::PushCanThread, this,6);
		DebugPrint(string(": Robot Movement : PUSHING CAN"));
	break;
	case RBT_TRACK_TURN:
		m_vision.ChangeVisionMode(VISION_SURVEAIL,30);
		m_servo_ctrl.setCameraDirection(TRK_LINE_CAM_PAN,TRK_LINE_CAM_TILT);
		m_lasttrack=Rect(m_trackregion.x ,m_trackregion.y, LINE_WIDTH*2, m_trackregion.height);
		m_lastx = m_trackregion.x + m_lasttrack.width/2;
		m_automode_thread = thread(&CRobotMgr::AutoTurnAroundThread, this,6);
		DebugPrint(string(": Robot Movement : U-TURN"));
	break;
	default:
		return false;
	}


	return false;

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

		if (newregion.width < LINE_WIDTH)
			newposition = newregion.x + newregion.width/2;
		else if ( (newposition-LINE_WIDTH/2) < newregion.x)
			newposition-=(newposition-LINE_WIDTH/2) - newregion.x;
		else if ( (newposition+LINE_WIDTH/2) > newregion.br().x)
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
	if (m_current_mode ==  mode)
		return;
	thread(&CRobotMgr::ChangeMode_Thread,this,mode);
}



void CRobotMgr::TrackMode_Thread(E_RBT_TRACK_SUB mode)
{
	ChangeRobotTrackMode(mode);
}


void CRobotMgr::CreateTrackModeThread(E_RBT_TRACK_SUB mode)
{

	if (mode==m_track_sub)
		return;
	thread(&CRobotMgr::TrackMode_Thread,this,mode);
}





void CRobotMgr::Assert_TrackMode(int error)
{
	YMessage sendMsg;
	printf("Lost Track \n");

	sendMsg = m_parser->getMessage(OP_SEND_MODE);
	sendMsg.setOpcode(OP_SEND_MODE);
	sendMsg.set("id", _id);
	sendMsg.set("event", "error");
	sendMsg.set("state", error);
	m_eventSender->send(sendMsg);

	CreateChangModeThread(RBT_NORMAL_MODE);
}


void CRobotMgr::onVisionImage(Mat & image)
{
	timespec currentTime;
	byte *data;
	uint length;

	if (!m_bImagesend)
		return;

	clock_gettime(CLOCK_REALTIME, &currentTime);

	if (getPassTime(m_prev_time, currentTime)<50)
		return;

	m_prev_time = currentTime;

	data = convertImageToJPEG(image,length);

    YMessage msg;
    msg.setOpcode(10);
    msg.set(data, length);

    m_imageSender->send(msg);
	
}


void CRobotMgr::onSignDetect(bool isdetected)
{
	timespec currentTime;
	long diff;
	long left,right;
	if (isdetected || m_track_sub!=RBT_TRACK_SIGN)
		return;


	m_signfailcount++;

	right = RBT_SIGN_RIGHT_PAN-((m_signfailcount/2)*5);
	left = RBT_SIGN_LEFT_PAN+(PAN_CAMERA_MIN-right);
	

	if (right>=PAN_CAMERA_MIN)
		m_servo_ctrl.setCameraDirection(right,SERVO_CENTER_OR_STOP);
	else if (left<PAN_CAMERA_MAX)
		m_servo_ctrl.setCameraDirection(left,SERVO_CENTER_OR_STOP);
	else
		Assert_TrackMode(1);
	this_thread::sleep(posix_time::millisec(100));

#if 0


	clock_gettime(CLOCK_REALTIME, &currentTime);
	diff = getPassTime(m_sign_time, currentTime);


	right = RBT_SIGN_RIGHT_PAN-(diff/10);
	left = RBT_SIGN_LEFT_PAN+(PAN_CAMERA_MIN-right);
	


	
	if (right>=PAN_CAMERA_MIN)
		m_servo_ctrl.setCameraDirection(right,SERVO_CENTER_OR_STOP);
	else if (left<PAN_CAMERA_MAX)
		m_servo_ctrl.setCameraDirection(left,SERVO_CENTER_OR_STOP);
	else
		Assert_TrackMode(1);

	this_thread::sleep(posix_time::millisec(100));
#endif
}


Rect CRobotMgr::onLineDetect(vector<Rect> & linelist)
{
	int dest_pt;
	double offsetfromcenter;
	Rect temprect;
	vector<Rect> templist;
	
	if(m_current_mode != RBT_TRACK_MODE)
		return Rect(0,0,0,0);


	switch(m_track_sub)
	{
		case RBT_TRACK_AUTO:


			if (!m_bRedbarDiscard && linelist.size() == 0)
			{
				m_unrecognization++;
				cout <<"no track "<< m_unrecognization<< " " << linelist.size()<<endl;
				if (m_unrecognization>3)
					Assert_TrackMode(3);
				return Rect(0,0,0,0);
			}
			else if (linelist.size() > 0)
			{
				m_unrecognization=0;
				templist = EliminateUselessPath(linelist);
				m_lasttrack = SelectPath(linelist,templist,m_track_direction,&dest_pt);
				m_lastx = dest_pt;
			}



			offsetfromcenter=1.0f - 2.0f*(float)m_lastx/(float)WIDTH;

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
	
		break;
		case RBT_TRACK_SIGN:
		break;
		case RBT_TRACK_PUSHCAN:
		case RBT_TRACK_AVOIDCAN:
		case RBT_TRACK_TURN:
			if (linelist.size() == 0)
				return Rect(0,0,0,0);
			
			templist = EliminateUselessPath(linelist);
			if (templist.size()>0)
				CreateTrackModeThread(RBT_TRACK_AUTO);

		break;
	}



	return Rect(0,0,0,0);


//	return m_lasttrack;
}



Vec3f CRobotMgr::onColorCircle(vector<cv::Vec3f> & circlelist)
{
	int i;

	for (i=0; i< circlelist.size(); i++)
	{
		if ((circlelist[i][0]) < m_lastx-(LINE_WIDTH)/2)
			m_track_direction=RBT_LEFT;
		if ((circlelist[i][0]) > m_lastx+(LINE_WIDTH)/2)
			m_track_direction=RBT_RIGHT;
	}
}
Rect CRobotMgr::onStopBar(vector<Rect> & linelist)
{
	int i;

	if (linelist.size() < 1)
	{
		m_bRedbarDiscard=false;
		return Rect(0,0,0,0);
	}
	if (m_bRedbarDiscard)
	{
		return Rect(0,0,0,0);
	}
	
	for (i=0; i< linelist.size(); i++)
	{
		if (linelist[i].width<200 && linelist[i].width>90)
		{
			if ((linelist[i].x + linelist[i].width/2) < m_lastx-(LINE_WIDTH)/2)
			{
				DebugPrint(string(": Track dot : LEFT DOT"));
				m_track_direction=RBT_LEFT;
			}
			if ((linelist[i].x + linelist[i].width/2) > m_lastx+(LINE_WIDTH)/2)
			{
				DebugPrint(string(": Track dot : RIGHT DOT"));
				m_track_direction=RBT_RIGHT;
			}
		}
		else if (linelist[i].width>400)
		{
//			m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 5);
//			m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 5);
//			this_thread::sleep(posix_time::millisec(300));
			m_bRedbarDiscard=true;
			DebugPrint(string(": Track bar : STOP"));
			CreateTrackModeThread(RBT_TRACK_SIGN);
			return linelist[i];
		}
	}

	
}


void CRobotMgr::onSignAction(int id, string action)
{

	cout <<"OnSignAction "<<action<<" " << id<<endl;
	switch(id)
	{
		case 0:
			DebugPrint(string(": Recognized Sign : STRAIGHT"));
			m_track_direction=RBT_STRAIGHT;
			CreateTrackModeThread(RBT_TRACK_AUTO);
		break;
		
		case 1:
			DebugPrint(string(": Recognized Sign : LEFT"));
			m_track_direction=RBT_LEFT;
			CreateTrackModeThread(RBT_TRACK_AUTO);
		break;

		case 2:
			DebugPrint(string(": Recognized Sign : RIGHT"));
			m_track_direction=RBT_RIGHT;
			CreateTrackModeThread(RBT_TRACK_AUTO);
		break;

		case 3:
			DebugPrint(string(": Recognized Sign : U-TURN"));
			CreateTrackModeThread(RBT_TRACK_TURN);
		break;

		case 4:
			DebugPrint(string(": Recognized Sign : PUSHCAN"));
			CreateTrackModeThread(RBT_TRACK_PUSHCAN);
		break;

		case 5:
			DebugPrint(string(": Recognized Sign : STOP"));
			CreateChangModeThread(RBT_NORMAL_MODE);
		break;
	}
}





long CRobotMgr::getPassTime(timespec StartTime, timespec CurrentTime)
{
	long StartTime_ms, CurrentTime_ms;

	StartTime_ms = StartTime.tv_sec * 1000 + (StartTime.tv_nsec /1000000);
	CurrentTime_ms = CurrentTime.tv_sec * 1000 + (CurrentTime.tv_nsec /1000000);

	return (CurrentTime_ms - StartTime_ms);
}

bool CRobotMgr::TurnCircular(int velocity, int r)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, BASESPEED + r);
	return m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, BASESPEED - velocity);
}


bool CRobotMgr::AvoidCanThread(int velocity)
{
	int step = 0;
	timespec stratTime , currentTime;

	clock_gettime(CLOCK_REALTIME, &stratTime);

	try{
		
		while(step < 5)
		{
			this_thread::interruption_point();
			switch(step)
			{
				case 0:
					clock_gettime(CLOCK_REALTIME, &currentTime);
					
					if(m_last_sonar > 20)
					{
						clock_gettime(CLOCK_REALTIME, &stratTime);
						step++;
					}
					else
					{
						TurnRight(2);
					}
					break;
		
				case 1:
					clock_gettime(CLOCK_REALTIME, &currentTime);
				
					if(getPassTime(stratTime, currentTime) > 320)
					{
						clock_gettime(CLOCK_REALTIME, &stratTime);
						step++;
					}
					else
					{
						TurnRight(6);
					}
				
					break;
				case 2:
					clock_gettime(CLOCK_REALTIME, &currentTime);
		
					if(getPassTime(stratTime, currentTime) > 1000)
					{
						clock_gettime(CLOCK_REALTIME, &stratTime);
						step++;
					}
					else
					{
						MoveForward(6);
					}
		
					break;
				
				case 3:
					clock_gettime(CLOCK_REALTIME, &currentTime);
		
					if(getPassTime(stratTime, currentTime) > 4000)
					{
						clock_gettime(CLOCK_REALTIME, &stratTime);
						step++;
					}
					else
					{
						if(getPassTime(stratTime, currentTime) > 3000)
							m_vision.ChangeVisionMode(VISION_TRACK,10);
						m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 2);
						m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 6);
					}
		
					break;
				case 4:
					clock_gettime(CLOCK_REALTIME, &currentTime);
					
					if(getPassTime(stratTime, currentTime) > 800)
					{
						clock_gettime(CLOCK_REALTIME, &stratTime);
						step++;
					}
					else
					{
						MoveForward(6);
					}
					break;
		
				default:
					break;
			}
			this_thread::sleep(posix_time::millisec(1));
		}
	}
	
	catch(thread_interrupted &)
	{
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
	}

	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);



}	


bool CRobotMgr::PushCanThread(int velocity)
{
	int step = 0;
	timespec stratTime , currentTime;
	clock_gettime(CLOCK_REALTIME, &stratTime);

	try{
		while(step < 2)
		{
			this_thread::interruption_point();
			switch(step)
			{
				case 0:
					clock_gettime(CLOCK_REALTIME, &currentTime);
					if(getPassTime(stratTime, currentTime) > 2000)
					{
						clock_gettime(CLOCK_REALTIME, &stratTime);
						step++;
					}
					else
					{
						MoveForward(6);
					}
		
					break;
				case 1:
					clock_gettime(CLOCK_REALTIME, &currentTime);
					if(getPassTime(stratTime, currentTime) > 2000)
					{
						clock_gettime(CLOCK_REALTIME, &stratTime);
						step++;
						AutoTurnAroundThread(6);
					}
					else
					{
						MoveBackward(6);
					}
					break;
		
				default:
					break;
			}
			this_thread::sleep(posix_time::millisec(1));
		}
	}

	
	catch(thread_interrupted &)
	{
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
	}
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
}	



bool CRobotMgr::AutoTurnAroundThread(int velocity)
{
	int step = 0;
	timespec stratTime , currentTime;

	clock_gettime(CLOCK_REALTIME, &stratTime);
	try{
		while(step < 2)
		{
			this_thread::interruption_point();
			switch(step)
			{
				case 0:
					clock_gettime(CLOCK_REALTIME, &currentTime);
					
					if(getPassTime(stratTime, currentTime) > 1000)
					{
						step++;
					}
					else
					{
						MoveForward(6);
					}
		
					break;
				case 1:
					clock_gettime(CLOCK_REALTIME, &currentTime);

					if(getPassTime(stratTime, currentTime) > 3000)
					{
						step++;
					}
					else
					{
						if(getPassTime(stratTime, currentTime) > 2000)
							m_vision.ChangeVisionMode(VISION_TRACK,10);
						TurnLeft(6);
					}
				
					break;
		
				default:
					break;
			}
			this_thread::sleep(posix_time::millisec(1));
		}

	}


	catch(thread_interrupted &)
	{
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
	}

	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
}	


bool CRobotMgr::TurnAroundThread(int velocity)
{
	int step = 0;
	timespec stratTime , currentTime;

	clock_gettime(CLOCK_REALTIME, &stratTime);
	try{
		while(step < 1)
		{
			this_thread::interruption_point();
			switch(step)
			{
				case 0:
					clock_gettime(CLOCK_REALTIME, &currentTime);
					
					if(getPassTime(stratTime, currentTime) > 2000)
					{
						step++;
					}
					else
					{
						TurnLeft(6);
					}
		
					break;
		
				default:
					break;
			}
			this_thread::sleep(posix_time::millisec(1));
		}

	}


	catch(thread_interrupted &)
	{
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
	}

	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
}	





bool CRobotMgr::MoveForward(int  velocity)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, velocity);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, velocity);

}

bool CRobotMgr::MoveBackward(int velocity)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, -velocity);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, -velocity);

}

bool CRobotMgr::TurnLeft(int velocity)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, -velocity);
	return m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, velocity);
}
bool CRobotMgr::TurnRight(int velocity)
{
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, velocity);
	return m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, -velocity);
}


bool CRobotMgr::SetCameraTilt(int degree)
{
	return m_servo_ctrl.setServoPosition(ENUM_SERVO_TILT, degree);
}
bool CRobotMgr::SetCameraPan(int degree)
{
	return m_servo_ctrl.setServoPosition(ENUM_SERVO_PAN, degree);
}



bool CRobotMgr::ManualMoveThread(E_RBT_MANUAL_MOVE move)
{
	try{
		while(1)
		{
			this_thread::interruption_point();
			switch(move)
			{
				case RBT_MAN_FORWARD:
					MoveForward(6);
				break;
				case RBT_MAN_BACKWARD:
					MoveBackward(6);
				break;
				case RBT_MAN_LEFT:
					TurnLeft(6);
				break;
				case RBT_MAN_RIGHT:
					TurnRight(6);
				break;
			}
			this_thread::sleep(posix_time::millisec(1));
		}

	}


	catch(thread_interrupted &)
	{
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
		m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
	}

	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_LEFT_WHEEL, 0);
	m_servo_ctrl.setWheelSpeed(ENUM_SERVO_RIGHT_WHEEL, 0);
}

bool CRobotMgr::CreateManualMove(E_RBT_MANUAL_MOVE move)
{
	StopManualMove();
	if (move ==  RBT_MAN_TURN)
		m_manual_thread = thread(&CRobotMgr::TurnAroundThread,this,6);
	else
		m_manual_thread = thread(&CRobotMgr::ManualMoveThread,this,move);
}
bool CRobotMgr::StopManualMove()
{
	if (m_manual_thread.get_id() !=  thread::id())
	{
		m_manual_thread.interrupt();
		m_manual_thread.join();
	}
}


void CRobotMgr::onReceiveDistance(double distanceCm)
{
	m_last_sonar = distanceCm;

	if(m_current_mode == RBT_TRACK_MODE && m_track_sub == RBT_TRACK_AUTO && distanceCm < 8.00)
	{
		cout<<"Sonar " << distanceCm << endl;
		ChangeRobotTrackMode(RBT_TRACK_AVOIDCAN);
	}
	
}

void CRobotMgr::onReceiveCommand(YMessage msg)
{
	cout << "[RobotManager] received command" << endl;
	int speed = 0;

	YMessage sendMsg;



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
		m_automode_thread = thread(&CRobotMgr::TurnAroundThread, this,6);

	break;
	case OP_RECV_CAMERA_CNTR_PAN:
		cout << "[RobotManager] " << msg.getString("command") << ", angle : " << msg.getInt("state") << endl;
		SetCameraPan(msg.getInt("state"));

	break;
	case OP_RECV_CAMERA_CNTR_TILT:
		cout << "[RobotManager] " << msg.getString("command") << ", angle : " << msg.getInt("state") << endl;
		SetCameraTilt(msg.getInt("state"));

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

		sendMsg = m_parser->getMessage(OP_SEND_MODE);
		sendMsg.setOpcode(OP_SEND_MODE);
		sendMsg.set("id", _id);
		sendMsg.set("event", "mode_changed");
		sendMsg.set("state", msg.getInt("state"));
		m_eventSender->send(sendMsg);
	break;

	case OP_RECV_IMAGE_SEND:
		cout << "[RobotManager] " << msg.getString("command") << ", state : " << msg.getInt("state") << endl;
		if (msg.getInt("state") == 1)
			m_bImagesend = true;
		else
			m_bImagesend = false;
		
	break;


	case OP_RECV_INIT_COMMAND:
		_id = msg.getInt("id");
		cout << "[RobotManager] Robot ID : " << _id <<endl;
	break;

	case OP_RECV_DEGUG:
		cout << "[RobotManager] " << msg.getString("debug") << ", state : " << msg.getString("state") << endl;

		DebugPrint(msg.getString("debug"));
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
    ChangeRobotMode(RBT_SUSPEND_MODE);
}

byte* CRobotMgr::convertImageToJPEG(Mat image, uint& length)
{
    byte* data;

    cv::imencode(".jpg", image, m_image_buffer, m_image_param);

    data = m_image_buffer.data();
    length = m_image_buffer.size();

    return data;
}


void CRobotMgr::DebugPrint(string message)
{
	YMessage sendMsg;

	if (_id == 0)
		return;
	
	sendMsg = m_parser->getMessage(OP_SEND_DEBUG);
	sendMsg.setOpcode(OP_SEND_DEBUG);
	sendMsg.set("id", _id);
	sendMsg.set("debug_info", message);
	sendMsg.set("state", 0);
	m_eventSender->send(sendMsg);
}
