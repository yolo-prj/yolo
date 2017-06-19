#ifndef _YOLO_IMAGE_PROCESSOR_H_
#define _YOLO_IMAGE_PROCESSOR_H_

#include <opencv2/core/core.hpp>


class ImageProcessor
{
public:
    ImageProcessor(){}
    ~ImageProcessor(){}
    static float findLineInImageAndComputeOffset(cv::Mat& image);

private:
    /*
    static const cv::Scalar ROI_COLOR   (  0.0, 255.0,   0.0);
    static const cv::Scalar TRACK_COLOR (  0.0,   0.0, 255.0);
    static const cv::Scalar SELECT_COLOR(255.0, 255.0, 255.0);
    static const cv::Scalar NAV_COLOR   (  0.0, 255.0, 255.0);
    */
    static cv::Scalar ROI_COLOR   ;
    static cv::Scalar TRACK_COLOR ;
    static cv::Scalar SELECT_COLOR;
    static cv::Scalar NAV_COLOR   ;
};



#endif
