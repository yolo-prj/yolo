#ifndef IMAGERECEIVERINTERFACE_H
#define IMAGERECEIVERINTERFACE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace yolo {
class ImageReceiveListener
{
public:
    virtual void OnImageReceived(cv::Mat&& image) = 0;
};
}

#endif // IMAGERECEIVERINTERFACE_H
