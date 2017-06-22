#include <yolo/YFramework/YCameraController.h>
#include <iostream>


namespace yolo
{

YCameraController::YCameraController()
{
    _captureThread = nullptr;
    _continueThread = false;
    _listener = nullptr;
    _capture = nullptr;
    _width = 640;
    _height = 480;
}

YCameraController::YCameraController(YCameraImageListener* listener)
{
    _captureThread = nullptr;
    _continueThread = false;
    _listener = listener;
    _capture = nullptr;
    _width = 640;
    _height = 480;
}

YCameraController::~YCameraController()
{
    stop();
}

void
YCameraController::start()
{
    _capture = cvCreateCameraCapture(0);
    if(_capture == nullptr) {
	cerr << "[CameraController] camera not initialized" << endl;
    }
    else
    {
	cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH, _width);
	cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT, _height);

	int width, height;
	width = cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH);
	height = cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT);

	cout << "[CameraController] Width = " << width << endl;
	cout << "[CameraController] Height = " << height << endl;

	_continueThread = true;
	_captureThread = new boost::thread(bind(&YCameraController::captureThread, this));
    }
}

void
YCameraController::stop()
{
    if(_captureThread != nullptr)
    {
	_continueThread = false;
	if(_captureThread->joinable())
	    _captureThread->join();
	delete _captureThread;
	_captureThread = nullptr;
    }

    if(_capture != nullptr)
    {
	cvReleaseCapture(&_capture);
	_capture = nullptr;
    }
}

void
YCameraController::captureThread()
{
    const int delay = 10;
    IplImage* iplCameraImage;
    Mat image;

    while(_continueThread)
    {
	iplCameraImage = cvQueryFrame(_capture);
	image = cv::cvarrToMat(iplCameraImage);
#ifdef ROBOT
	flip(image, image, -1);
#endif

	if(_listener != nullptr)
	    _listener->onReceiveImage(image);

	boost::this_thread::sleep(boost::posix_time::milliseconds(20));
    }
}

}
