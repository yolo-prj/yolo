
#include <iostream>

#include <boost/thread.hpp>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "PI3OpencvCompat.h"



#include "LineDetect.h"

#include "Vision.h"


using namespace boost;
using namespace cv;
using namespace std;


#define REFRESH_RATE	(50)
#define TIMER_ID		(0)

CVision::CVision()
{
	if (!m_single)
		m_single = this;
	m_vision_interface=NULL;

	m_cam_width=640;
	m_cam_height=480;
	m_cam_track=Rect(10, 2*m_cam_height/3, m_cam_width-20, m_cam_height/12);



	black_min=cv::Scalar(0, 0, 0);
	black_max=cv::Scalar(179, 100, 80);

	red_min=cv::Scalar(160, 100, 100);
	red_max=cv::Scalar(179, 179, 179);

	
	m_capture = Start_Camera();
	if(!m_capture)
	{
		printf("Camera fail\n");
	}


	

}

CVision::CVision(CVisionInf *intf)
{
	if (!m_single)
		m_single = this;
	m_vision_interface=intf;


	m_cam_width=640;
	m_cam_height=480;
	m_cam_track=Rect(10, 2*m_cam_height/3, m_cam_width-20, m_cam_height/12);

	black_min=cv::Scalar(0, 0, 0);
	black_max=cv::Scalar(179, 100, 80);

	red_min=cv::Scalar(160, 100, 100);
	red_max=cv::Scalar(179, 179, 179);


	m_capture = Start_Camera();
	if(!m_capture)
	{
		printf("Camera fail\n");
	}
}

CVision::~CVision()
{
	if (m_single==this)
		m_single = NULL;

	Stop_Vision();
	Stop_Camera(m_capture);
}


void CVision::SetInterface(CVisionInf *intf)
{
	m_vision_interface=intf;
}


void CVision::SetCameraParam(int width, int height, Rect track_region)
{
	m_cam_width=width;
	m_cam_height=height;
	m_cam_track=track_region;
}



void CVision::ChangeVisionMode(E_VISION_MODE mode,int refresh_rate)
{
	Stop_Vision();
	if (mode != VISION_STOP)
		Start_Vision(mode, refresh_rate, TIMER_ID);
}





void CVision::Start_Vision(E_VISION_MODE mode, int msec, int timer_id)
{
	m_vision_thread = thread(&CVision::Vision_Thread,this, mode,msec,timer_id);
}


void CVision::Stop_Vision()
{
	m_vision_thread.interrupt();
	m_vision_thread.join();
}



CvCapture * CVision::Start_Camera()
{
	CvCapture *    capture=NULL;	

	capture =cvCreateCameraCapture(0);   // Open default Camera
	if(capture)
	{
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, m_cam_width);
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, m_cam_height);
	}

	return capture;
}



void CVision::Stop_Camera(CvCapture* capture)
{
	if(!capture)
		cvReleaseCapture(&capture); 
}


void CVision::Vision_Thread(E_VISION_MODE mode, int msec, int sig_id)
{
	IplImage * iplCameraImage;
	Mat 	   image;
	Rect 	   track_rec;



	if (!m_capture)
	{
		printf("Camera fail\n");
		return;
	
	}

	printf("Vision thread start %d, %d\n",msec, sig_id);
	Timer_Start(msec,sig_id);


#if(!IsPi3)

	namedWindow( "camera", CV_WINDOW_AUTOSIZE );
	namedWindow( "camera2", CV_WINDOW_AUTOSIZE );

	iplCameraImage = cvQueryFrame(m_capture); // Get Camera image
	image= cv::cvarrToMat(iplCameraImage);
	imshow("camera", image );
	imshow("camera2", image );
#endif
	try
	{
		while(1)
		{
			unique_lock<boost::mutex> lock(m_mut);
			m_cond.wait(lock);

			iplCameraImage = cvQueryFrame(m_capture); // Get Camera image
			image= cv::cvarrToMat(iplCameraImage);	// Convert Camera image to Mat format
			flip(image, image,-1);		// if running on PI3 flip(-1)=180 degrees


			
			switch (mode)
			{
			case VISION_TRACK:
				RecogTrack(image, m_cam_track);
				break;
				
			case VISION_SIGN:
				RecogSign(image);
				break;

			case VISION_SURVEAIL:
				CameraSurveillance(image);
				break;
			}

#if(!IsPi3)
			imshow("camera", image );
			cv::waitKey(1);			
#else
	if (m_vision_interface)
		m_vision_interface->onVisionImage(image);
#endif
		}

	}

	catch(thread_interrupted &)
	{
		timer_delete(m_timer_id);
		printf("Vision thread exit \n");
	}
	
}





void CVision::RecogTrack(Mat & camimage, Rect rect)
{
	char text[1024];
	float offsetfromcenter;
	vector<Rect> detectline,stopbar;
	vector<cv::Vec3f> directtion_circle;
	Vec3f lastcircle;
	Rect lastline,laststop;
	int count;
	int i;


	static const cv::Scalar ROI_COLOR	(  0.0, 255.0,	 0.0);
	static const cv::Scalar TRACK_COLOR (  0.0,   0.0, 255.0);
	static const cv::Scalar SELECT_COLOR(255.0, 255.0, 255.0);
	static const cv::Scalar NAV_COLOR	(  0.0, 255.0, 255.0);


	m_line_detector.SetRegion(rect);
	m_line_detector.SetParam( cv::Scalar(0, 0, 0), cv::Scalar(179, 100, 80),80.0,640.0);
	count = m_line_detector.GetBar(camimage,detectline,2);

	m_line_detector.SetParam( cv::Scalar(160, 100, 100), cv::Scalar(179, 179, 179),200.0,300.0);
	count = m_line_detector.GetBar(camimage,stopbar,1);

	m_line_detector.SetParam( cv::Scalar(160, 100, 100), cv::Scalar(179, 179, 179),50.0,100.0);
	count = m_line_detector.GetCircle(camimage,directtion_circle,1);


	if (m_vision_interface)
		lastline = m_vision_interface->onLineDetect(detectline);
	if (m_vision_interface)
		laststop = m_vision_interface->onStopBar(stopbar);

	if (m_vision_interface)
		lastcircle = m_vision_interface->onColorCircle(directtion_circle);

	for(i = 0; i<detectline.size();i++)	 
	{
		rectangle(camimage, detectline[i], cv::Scalar(200,200,200),2); 
	}


	for(i = 0; i<stopbar.size();i++)	 
	{
		rectangle(camimage, stopbar[i], cv::Scalar(20,200,20),2); 
	}

	for(i = 0; i<directtion_circle.size();i++)	 
	{
		circle(camimage, Point(directtion_circle[i][0],directtion_circle[i][1]),directtion_circle[i][2], cv::Scalar(20,200,20),2); 
	}


	if(lastline.y != 0)
	{
		rectangle(camimage, lastline, cv::Scalar(200,20,20),2); 
		line(camimage,Point(lastline.x+lastline.width/2,lastline.y),Point(lastline.x+lastline.width/2,lastline.y+lastline.height),Scalar( 200,200,20 ),2,8);
	}



	line( camimage,Point(camimage.cols/2,0),Point(camimage.cols/2,camimage.rows),Scalar( 0, 0, 0 ),2,8);



}


void CVision::RecogSign(Mat & camimage)
{

}


void CVision::CameraSurveillance(Mat & camimage)
{
	vector<cv::Vec3f> directtion_circle;
	m_line_detector.SetRegion(Rect(0,0,camimage.cols,camimage.rows));
	m_line_detector.SetParam( red_min, red_max,50.0,100.0);
	m_line_detector.GetCircle(camimage,directtion_circle,1);

}




void CVision::Timer_Handler(int sig, siginfo_t *si, void *uc )
{
	if (m_single)
		m_single->m_cond.notify_one();
}

bool CVision::Timer_Start(int msec, int signo)
{  
    struct sigevent         te;  
    struct itimerspec       its;  
    struct sigaction        signal_set;  


	signo = SIGUSR1+signo;

    signal_set.sa_flags = SA_SIGINFO;  
    signal_set.sa_sigaction =  &this->Timer_Handler;  
    sigemptyset(&signal_set.sa_mask);  
  
    if (sigaction(signo, &signal_set, NULL) == -1)  
    {  
        printf("sigaction error\n");
        return false;  
    }  
   
    te.sigev_notify = SIGEV_SIGNAL;  
    te.sigev_signo = signo;  
    te.sigev_value.sival_ptr = &(m_timer_id);  

    timer_create(CLOCK_REALTIME, &te, &(m_timer_id));  
   
    its.it_interval.tv_sec = msec/1000;
    its.it_interval.tv_nsec = (msec%1000) * 1000000;  
    its.it_value.tv_sec = msec/1000;
    its.it_value.tv_nsec = (msec%1000) * 1000000;

    timer_settime(m_timer_id, 0, &its, NULL);  
   
    return true;  
}	


CVision * CVision::m_single = NULL;


