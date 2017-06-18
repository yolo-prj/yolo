#ifndef _YOLO_CAMERA_CONTROLLER_H_
#define _YOLO_CAMERA_CONTROLLER_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/thread.hpp>
#include <yolo/YFramework/YCameraImageListener.h>


using namespace std;
using namespace cv;

namespace yolo
{
class YCameraController
{
public:
    YCameraController();
    YCameraController(YCameraImageListener*);
    ~YCameraController();

    void start();
    void stop();

private:
    void init();
    void captureThread();

private:
    YCameraImageListener*   _listener;
    boost::thread*	    _captureThread;
    bool		    _continueThread;
    CvCapture*		    _capture;

    int			    _width;
    int			    _height;
};

}


#endif
