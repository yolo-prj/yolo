#ifndef __VISION_CLASS__
#define __VISION_CLASS__


#include <boost/thread.hpp>
#include <signal.h>



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "LineDetect.h"
#include "VisionInf.h"


#include "PI3OpencvCompat.h"

using namespace boost;
using namespace cv;



typedef enum 
{
	VISION_STOP =0,
	VISION_TRACK,
	VISION_SIGN,
	VISION_SURVEAIL,
}E_VISION_MODE;


class CVision
{
private : 
	thread m_vision_thread;
	timer_t m_timer_id;
	boost::condition_variable m_cond;
	boost::mutex m_mut;
	bool m_data_ready;







	CvCapture *    m_capture;

	CLineDetector m_line_detector;
	CVisionInf * m_vision_interface;

public:
	static CVision * m_single;
	int m_cam_width;
	int m_cam_height;
	Rect m_cam_track;


	cv::Scalar black_min,black_max;
	cv::Scalar red_min,red_max;


	CVision();
	CVision(CVisionInf *intf);
	~CVision();


public:
	void SetCameraParam(int width, int height, Rect track_region);

	void SetInterface(CVisionInf *intf);
	void ChangeVisionMode(E_VISION_MODE mode,int refresh_rate);


	



	void RecogTrack(Mat & camimage, Rect rect);
	void RecogSign(Mat & camimage);
	void CameraSurveillance(Mat & camimage);



private:

	CvCapture * Start_Camera();
	void Stop_Camera(CvCapture* capture);

	void Start_Vision(E_VISION_MODE mode, int msec, int timer_id);
	void Stop_Vision();

	void Vision_Thread(E_VISION_MODE mode, int msec, int sig_id);

	static void Timer_Handler(int sig, siginfo_t *si, void *uc );
	bool Timer_Start(int msec, int signo);
	

};



#endif
