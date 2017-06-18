//------------------------------------------------------------------------------------------------
// File: PI3OpencvCompat.h
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Provides compatibility between rapicam and opencv so the same opencv API can be used seamlessly
//------------------------------------------------------------------------------------------------

#ifndef PI3OpencvCompatH
#define PI3OpencvCompatH
#ifdef __arm__
#define PI
#endif

#ifdef PI
#include "RaspiCamCV.h"
#define  IsPi3 1
#define  CvCapture                RaspiCamCvCapture
#define  cvCreateCameraCapture    raspiCamCvCreateCameraCapture
#define  cvQueryFrame             raspiCamCvQueryFrame
#define  cvReleaseCapture         raspiCamCvReleaseCapture
#define  cvSetCaptureProperty     raspiCamCvSetCaptureProperty
#define  cvGetCaptureProperty     raspiCamCvGetCaptureProperty
#define  CV_CAP_PROP_FRAME_WIDTH  RPI_CAP_PROP_FRAME_WIDTH
#define  CV_CAP_PROP_FRAME_HEIGHT RPI_CAP_PROP_FRAME_HEIGHT

#else
#define  IsPi3 0
#endif

#endif
//------------------------------------------------------------------------------------------------
//END of Include
//------------------------------------------------------------------------------------------------

