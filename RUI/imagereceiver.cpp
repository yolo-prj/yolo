#include "imagereceiver.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <chrono>

#include <QDebug>

using namespace std;
using namespace cv;

namespace yolo {

ImageReceiver::ImageReceiver()
{
    image_message_listener_ = std::make_shared<ImageMessageListener>();
    image_message_listener_->SetParent(this);
}

ImageReceiver::~ImageReceiver()
{

}

int ImageReceiver::StartReceiving()
{
    qDebug() << "StartReceiving()";
    return 0;
}

int ImageReceiver::StopReceiving()
{
    qDebug() << "StopReceiving()";
    return 0;
}

bool ImageReceiver::ReceiveImageAsJpeg(byte* data, uint length, cv::Mat* image)
{
    cv::imdecode(cv::Mat(length,1,CV_8UC1,data), cv::IMREAD_COLOR, image );
    if (!(*image).empty()) return true;
    else return false;

    return false;
}

void ImageReceiver::TransferImage(byte* data, uint length)
{
    std::thread worker = std::thread([&]() {
            cv::Mat image, resized;
            this->ReceiveImageAsJpeg(data+8, length-8, &image);

            //Size size(320*2,240*2);
            //cv::resize(image,resized,size);
            // OpenCV to QImage datatype to display on labels
            cv::cvtColor(image,image,CV_BGR2RGB);

            this->image_receive_listener_->OnImageReceived(std::move(image));
        });

    worker.join();
}

void ImageReceiver::SetNetworkManager(const std::shared_ptr<YNetworkManager>& manager)
{
    manager_ = manager;
    manager_->addNetworkMessageListener(10, image_message_listener_.get());
}

void ImageReceiver::RegisterListener(const std::shared_ptr<ImageReceiveListener>& interface)
{
    image_receive_listener_ = interface;
}

void ImageReceiver::UnregisterListener()
{
    image_receive_listener_.reset();
}

void ImageReceiver::SetTimePeriod(int ms)
{
    time_period_ = ms;
}

void ImageReceiver::ImageMessageListener::onReceiveMessage(byte* data, uint length)
{
    //qDebug() << "message received :" << length;
    parent_->TransferImage(data, length);
    //delete[] data;
}

}

