
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



#include "LineDetect.h"
#include "PI3OpencvCompat.h"


using namespace cv;
using namespace std;

CLineDetector::CLineDetector()
{
	m_region = Rect(0,0,0,0);
	m_hsv_start = Scalar(0,0,0);
	m_hsv_end = Scalar(0,0,0);

	m_min = 0.0;
	m_max = 0.0;
}


CLineDetector::~CLineDetector()
{
}

void CLineDetector::SetRegion(Rect rc)
{
	m_region = rc;
}

Rect CLineDetector::GetRegion()
{
	return m_region;
}



void CLineDetector::SetParam(Scalar startshv, Scalar endshv, double min, double max )
{
	m_hsv_start = startshv;
	m_hsv_end = endshv;

	m_min = min;
	m_max = max;
}


int CLineDetector::GetBlackBar(Mat &image, vector<Rect> & coutrect, int bar_max)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Rect rc_bound;
	Mat mono,blur,thresh,erodeImg,dilateImg;
	Mat hsv;

	int ct;

	Mat roi(image,m_region); 

	
	cvtColor(roi,hsv,COLOR_BGR2HSV);
	inRange(hsv, m_hsv_start, m_hsv_end, mono);


	GaussianBlur(mono,blur, Size(5,5),0);
	Mat erodeElmt1 = getStructuringElement(MORPH_RECT, Size(3,3));
	Mat dilateElmt1 = getStructuringElement(MORPH_RECT, Size(9,9));
	dilate(blur, dilateImg,dilateElmt1);
	morphologyEx(dilateImg,erodeImg, MORPH_CLOSE, dilateElmt1);
	
	findContours(erodeImg, contours, hierarchy, RETR_LIST,CHAIN_APPROX_SIMPLE);


	coutrect.clear();
	
	for(unsigned int i = 0; i<contours.size();i++)	
	{
		rc_bound = boundingRect(contours[i]);

		rc_bound = rc_bound + m_region.tl();
		
//		cout << " bar "<< rc_bound << endl;
		if (rc_bound.width > m_min && rc_bound.width < m_max && rc_bound.height>5) //rc_bound.height>m_region.height/2) 
		{
//			cout << boundingRect(contours[i]) << endl;
			for(ct=0; ct< (bar_max)?bar_max:coutrect.size(); ct++)
			{
				if (ct>=coutrect.size())
					break;
				if (coutrect[ct].width<rc_bound.width)
					break;
			}
			coutrect.insert(coutrect.begin()+ct, rc_bound);
		}

		rectangle(erodeImg, boundingRect(contours[i]), Scalar(100,100,100),3);
	}

#if(!IsPi3)
	imshow("camera2", erodeImg );
#endif
	

	if (bar_max && bar_max<coutrect.size())
		coutrect.erase(coutrect.begin()+bar_max,coutrect.end());

//	erodeImg.copyTo(image);
	
	return coutrect.size();

}



int CLineDetector::GetColorBar(Mat &image, Scalar startshv, Scalar endshv, vector<Rect> & coutrect, int bar_max)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Rect rc_bound;
	Mat mono,blur,thresh,erodeImg,dilateImg,temp;
	Mat hsv;

	int ct;

	Mat roi(image,m_region); 

	
	cvtColor(roi,hsv,COLOR_BGR2HSV);
	inRange(hsv, m_hsv_start, m_hsv_end, mono);
	inRange(hsv, startshv, endshv, temp);
    add(mono, temp,mono);


	GaussianBlur(mono,blur, Size(5,5),0);
	Mat erodeElmt1 = getStructuringElement(MORPH_RECT, Size(3,3));
	Mat dilateElmt1 = getStructuringElement(MORPH_RECT, Size(9,9));
	dilate(blur, dilateImg,dilateElmt1);
	morphologyEx(dilateImg,erodeImg, MORPH_CLOSE, dilateElmt1);
	
	findContours(erodeImg, contours, hierarchy, RETR_LIST,CHAIN_APPROX_SIMPLE);


	coutrect.clear();
	
	for(unsigned int i = 0; i<contours.size();i++)	
	{
		rc_bound = boundingRect(contours[i]);

		rc_bound = rc_bound + m_region.tl();
		
//		cout << " bar "<< rc_bound << endl;
		if (rc_bound.width > m_min && rc_bound.width < m_max && rc_bound.height>5) //rc_bound.height>m_region.height/2) 
		{
//			cout << boundingRect(contours[i]) << endl;
			for(ct=0; ct< (bar_max)?bar_max:coutrect.size(); ct++)
			{
				if (ct>=coutrect.size())
					break;
				if (coutrect[ct].width<rc_bound.width)
					break;
			}
			coutrect.insert(coutrect.begin()+ct, rc_bound);
		}

		rectangle(erodeImg, boundingRect(contours[i]), Scalar(100,100,100),3);
	}

#if(!IsPi3)
	imshow("camera2", erodeImg );
#endif
	

	if (bar_max && bar_max<coutrect.size())
		coutrect.erase(coutrect.begin()+bar_max,coutrect.end());

//	erodeImg.copyTo(image);
	
	return coutrect.size();

}





int CLineDetector::GetCircle(Mat &image, vector<cv::Vec3f> & coutrect, int bar_max)
{
	Vec3f sel_circle;
	
	Mat mono,blur,thresh,erodeImg,dilateImg;
	Mat hsv;

	int ct;

	Mat roi(image,m_region); 
	cvtColor(roi,hsv,COLOR_BGR2HSV);
	inRange(hsv, m_hsv_start, m_hsv_end, mono);



	Mat erodeElmt1 = getStructuringElement(MORPH_RECT, Size(3,3));
	Mat dilateElmt1 = getStructuringElement(MORPH_RECT, Size(5,5));



	GaussianBlur(mono,erodeImg, Size(5,5),0);

	erode(erodeImg, dilateImg,dilateElmt1);
	morphologyEx(dilateImg,erodeImg, MORPH_CLOSE, dilateElmt1);

	
	 std::vector<cv::Vec3f> circles;
	
	
	 cv::HoughCircles(erodeImg, circles, CV_HOUGH_GRADIENT, 1, erodeImg.rows/8, 100, 20, 0, 0);


	coutrect.clear();

	for(unsigned int i = 0; i<circles.size();i++)	
	{
		sel_circle = circles[i];
		sel_circle[0] = sel_circle[0] + m_region.x;
		sel_circle[1] = sel_circle[1] + m_region.y;
		
		if (sel_circle[2] > m_min && sel_circle[2] < m_max) 
		{
			for(ct=0; ct< (bar_max)?bar_max:coutrect.size(); ct++)
			{
				if (ct>=coutrect.size())
					break;
				if (coutrect[ct][2]<sel_circle[2])
					break;
			}
			coutrect.insert(coutrect.begin()+ct, sel_circle);
		}
		circle(erodeImg, Point(sel_circle[0],sel_circle[1]),sel_circle[2], Scalar(100,100,100),3);

	}



#if(!IsPi3)
	imshow("camera2", erodeImg );
#endif



	if (bar_max<coutrect.size())
		coutrect.erase(coutrect.begin()+bar_max,coutrect.end());

	return coutrect.size();

}

