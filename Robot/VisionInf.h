#ifndef __VISIONINF_CLASS__
#define __VISIONINF_CLASS__


#include <signal.h>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace cv;
using namespace std;

class CVisionInf
{

public:
	CVisionInf(){}
	~CVisionInf(){}

public:

	virtual void onVisionImage(Mat & image)
	{
	}


	virtual Rect onLineDetect(vector<Rect> & linelist)
	{
	}

	virtual Vec3f onColorCircle(vector<cv::Vec3f> & circlelist)
	{
	}

	virtual Rect onStopBar(vector<Rect> & linelist)
	{
	}

	virtual void onSignDetect(bool isdetected)
	{
	}


};



#endif
