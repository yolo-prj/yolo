
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

CVision::CVision(CVisionInf *intf,vector<CSignImage *> * signlist)
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


	m_signlist = signlist;

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


void CVision::SetInterface(CVisionInf *intf,vector<CSignImage *> * signlist)
{
	m_vision_interface=intf;
	m_signlist = signlist;
}


void CVision::SetCameraParam(int width, int height, Rect track_region)
{
	m_cam_width=width;
	m_cam_height=height;
	m_cam_track=track_region;
}



void CVision::ChangeVisionMode(E_VISION_MODE mode,int refresh_rate)
{
	if (mode == mode)
		return;
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
	cout<<"join Stop_Vision"<<endl;
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
	Mat 	   hsv;
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
//			cvtColor(image,hsv,COLOR_BGR2HSV);
			
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


//			cvtColor(hsv,image,COLOR_HSV2BGR);


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


//	cout<<"<< black >>"<< rect <<" "<<endl;
	m_line_detector.SetRegion(rect);
	m_line_detector.SetParam( cv::Scalar(0, 0, 0), cv::Scalar(179, 255, 80),100.0,640.0);
	count = m_line_detector.GetBlackBar(camimage,detectline,2);
//	cout<<"<< black >>"<<endl;

//	cout<<"<< red >>"<<endl;
	m_line_detector.SetParam( cv::Scalar(0, 50, 00), cv::Scalar(15, 255, 255),050.0,600.0);
	count = m_line_detector.GetColorBar(camimage,cv::Scalar(165, 50, 00), cv::Scalar(179, 255, 255),stopbar,0);
//	cout<<"<< red >>"<<endl;

//	m_line_detector.SetParam( cv::Scalar(0, 100, 100), cv::Scalar(15, 255, 255),50.0,400.0);
//	count = m_line_detector.GetCircle(camimage,directtion_circle,1);


	if (m_vision_interface)
		lastline = m_vision_interface->onLineDetect(detectline);

	if (m_vision_interface)
		laststop = m_vision_interface->onStopBar(stopbar);


	
//	if ( m_vision_interface)
//		lastcircle = m_vision_interface->onColorCircle(directtion_circle);

	for(i = 0; i<detectline.size();i++)	 
	{
		rectangle(camimage, detectline[i], cv::Scalar(200,200,200),2); 
	}

	for(i = 0; i<stopbar.size();i++)	 
	{
		if (stopbar[i].width > 400)
			rectangle(camimage, stopbar[i], cv::Scalar(20,200,20),2); 
		if ((stopbar[i].width > 90) && (stopbar[i].width < 200))
			circle(camimage, Point(stopbar[i].x+stopbar[i].width/2, stopbar[i].y+stopbar[i].height/2),stopbar[i].width/2, cv::Scalar(20,200,20),2); 
	}

//	for(i = 0; i<directtion_circle.size();i++)	 
//	{
//		circle(camimage, Point(directtion_circle[i][0],directtion_circle[i][1]),directtion_circle[i][2], cv::Scalar(20,200,20),2); 
//	}


	if(lastline.y != 0)
	{
		rectangle(camimage, lastline, cv::Scalar(200,20,20),2); 
		line(camimage,Point(lastline.x+lastline.width/2,lastline.y),Point(lastline.x+lastline.width/2,lastline.y+lastline.height),Scalar( 200,200,20 ),2,8);
	}



	line( camimage,Point(camimage.cols/2,0),Point(camimage.cols/2,camimage.rows),Scalar( 0, 0, 0 ),2,8);



}


double CVision::median(cv::Mat channel)
{
	double m = (channel.rows*channel.cols) / 2;
	int    bin = 0;
	double med = -1.0;

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	cv::Mat hist;
	cv::calcHist(&channel, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

	for (int i = 0; i < histSize && med < 0.0; ++i)
	{
		bin += cvRound(hist.at< float >(i));
		if (bin > m && med < 0.0)
			med = i;
	}

	return med;
}

void CVision::sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;

	for (unsigned int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}


void CVision::RecogSign(Mat & camimage)
{
	Mat 			   greyImg, new_image;
	double med;
	double sigma = 0.33;
	int lower, upper;

	cvtColor(camimage, greyImg, CV_RGB2GRAY);

	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	GaussianBlur(greyImg, greyImg, Size(9, 9), 2, 2);


	/// Detect edges using canny
	med = median(greyImg);
	lower = int(max(0, (int)((1.0 - sigma) * med)));
	upper = int(min(255, (int)((1.0 + sigma) * med)));
	Canny(greyImg, canny_output, lower, upper);


	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE,
	CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<Point> approxRect;

	for (size_t i = 0; i < contours.size(); i++) 
	{
		approxPolyDP(contours[i], approxRect,
		arcLength(Mat(contours[i]), true) * 0.01, true);

		if (approxRect.size() == 4) 
		{
			double area = contourArea(contours[i]);
 
			if (area > 2000)

			{
				std::vector<cv::Point2f> corners;
				double minVal, maxVal, medVal;
				vector<Point>::iterator vertex;
				Moments mu;
				// Define the destination image
				Mat correctedImg = ::Mat::zeros(195, 271, CV_8UC3);
				Mat diffImg;

				int match, minDiff, diff;
				Mat correctedImgBin;
				Mat transmtx;

				
				vertex = approxRect.begin();
				//vertex++;
				circle(camimage, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				circle(camimage, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				circle(camimage, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				circle(camimage, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);

				mu = moments(contours[i], false);

				sortCorners(corners, Point2f((float)(mu.m10 / mu.m00), (float)(mu.m01 / mu.m00)));


				// Corners of the destination image
				std::vector<cv::Point2f> quad_pts;
				quad_pts.push_back(Point2f(0, 0));
				quad_pts.push_back(Point2f((float)correctedImg.cols, 0.0f));
				quad_pts.push_back(Point2f((float)correctedImg.cols, (float)correctedImg.rows));
				quad_pts.push_back(Point2f(0.0f, (float)correctedImg.rows));

				// Get transformation matrix
				transmtx = getPerspectiveTransform(corners, quad_pts);

				// Apply perspective transformation
				warpPerspective(camimage, correctedImg, transmtx,
				correctedImg.size());


				cvtColor(correctedImg, correctedImgBin, CV_RGB2GRAY);

				//equalizeHist(correctedImgBin, correctedImgBin);

				correctedImgBin.copyTo(new_image);

				threshold(correctedImgBin, correctedImgBin, 140, 255, 0);


				minMaxLoc(new_image, &minVal, &maxVal);

				medVal = (maxVal - minVal) / 2;

				threshold(new_image, new_image, medVal, 255, 0);


				minDiff = 12000;
				match = -1;

				for (int i = 0; i <m_signlist->size(); i++)
				{
					diff=(*m_signlist)[i]->GetDiff(new_image);

					if (diff < minDiff) 
					{
						minDiff = diff;
						match = i;
					}

 				}
 
				if (match != -1) 
				{
					printf("Match %s\n", (*m_signlist)[match]->m_actionname.c_str());
					(*m_signlist)[match]->CallHandler();
					if (m_vision_interface)
						m_vision_interface->onSignDetect(true);
					return;
					
 				}
				else
				{	
					printf("No Match\n");
					if (m_vision_interface)
						m_vision_interface->onSignDetect(false);
					return;
				}


			}

		}
	}



}


void CVision::CameraSurveillance(Mat & camimage)
{
#if 0
	vector<Rect> detectline,stopbar;
	m_line_detector.SetRegion(Rect(0,0,camimage.cols,camimage.rows));
	m_line_detector.SetParam( cv::Scalar(0, 50, 00), cv::Scalar(15, 255, 255),100.0,700.0);
	m_line_detector.GetBar(camimage,stopbar,0);

	Mat temp;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Mat mono,blur,thresh,erodeImg,dilateImg;
	Mat hsv;


	cvtColor(camimage,temp,COLOR_BGR2HSV);
	inRange(temp, cv::Scalar(0, 50, 00), cv::Scalar(15, 255, 255), mono);



	GaussianBlur(mono,blur, Size(5,5),0);
	Mat erodeElmt1 = getStructuringElement(MORPH_RECT, Size(3,3));
	Mat dilateElmt1 = getStructuringElement(MORPH_RECT, Size(9,9));
	dilate(blur, dilateImg,dilateElmt1);
	morphologyEx(dilateImg,erodeImg, MORPH_CLOSE, dilateElmt1);
	
	findContours(erodeImg, contours, hierarchy, RETR_LIST,CHAIN_APPROX_SIMPLE);


//	cvtColor(camimage,camimage,COLOR_HSV2RGB);

	for(int i = 0; i<contours.size();i++)	 
	{
//		cout << boundingRect(contours[i]) << endl;
		rectangle(camimage, boundingRect(contours[i]), cv::Scalar(20,200,20),2); 
	}

/*

	vector<cv::Vec3f> directtion_circle;
	m_line_detector.SetRegion(Rect(0,0,camimage.cols,camimage.rows));
	m_line_detector.SetParam( red_min, red_max,50.0,100.0);
	m_line_detector.GetCircle(camimage,directtion_circle,1);
*/
#endif
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


