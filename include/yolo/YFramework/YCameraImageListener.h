#ifndef _YOLO_CAMERA_IMAGE_LISTENER_H_
#define _YOLO_CAMERA_IMAGE_LISTENER_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;

namespace yolo
{

class YCameraImageListener
{
public:
    virtual void onReceiveImage(Mat image) = 0;
};

}


#endif
