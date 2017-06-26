#ifndef __LINEDETECT_CLASS__
#define __LINEDETECT_CLASS__


#include <boost/thread.hpp>
#include <signal.h>



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace cv;
using namespace std;

class CLineDetector
{
private : 
	Rect m_region;
	
	Scalar m_hsv_start;
	Scalar m_hsv_end;
	double m_min;
	double m_max;

public:
	CLineDetector();
	~CLineDetector();

public:
	void SetRegion(Rect rc);
	Rect GetRegion();
	void SetParam(Scalar startshv, Scalar endshv, double min, double max );
	int GetBar(Mat &image, vector<Rect> & coutrect, int bar_max);
	int GetCircle(Mat &image, vector<cv::Vec3f> & coutrect, int bar_max);
	
};



#endif
