#ifndef RUIMODEL_H
#define RUIMODEL_H

#include <queue>
#include <mutex>
#include <memory>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QString>
#include <QObject>

#include "imagereceivelistener.h"
#include "roboteventlistener.h"
#include "robotmode.h"

#include <QDebug>

using namespace std;
using namespace yolo;

class RUIModel : public QObject
{

    Q_OBJECT

signals:
    void RobotStatusChanged(int a);

public:
    explicit RUIModel(QObject *parent = 0);
    ~RUIModel();

    static RUIModel* GetInstance();
    virtual void Connect();
    virtual void Disconnect();

    virtual int GetRobotStatus();

    virtual int GetImage(cv::Mat *Image);

    virtual void HandleRobotOperation();

    virtual void SetRobotMode(RobotMode mode);

    virtual void HandlePanOperation();
    virtual void HandleTiltOperation();

    // signal slot test
    void CreateSignal();

    bool IsEmpty() const;
    int PushImage(cv::Mat&& image);
    cv::Mat GetImage();

    void SetRobotHandler(int handle) { cur_robot_handle_ = handle; }

private:
    std::queue<cv::Mat> image_queue_;
    std::mutex image_mutex_;
    int cur_robot_handle_;

    class ImageReceiveListenerImp : public yolo::ImageReceiveListener {
        private:
            RUIModel* ruimodel_;

        public:
        ImageReceiveListenerImp(RUIModel* ruimodel) : ruimodel_(ruimodel) {

        }

        virtual void OnImageReceived(cv::Mat&& image) override
        {
            ruimodel_->PushImage(std::move(image));
        }
    };

    class RobotEventListenerImp : public yolo::RobotEventListener {
        private:
            RUIModel* ruimodel_;

        public:
        RobotEventListenerImp(RUIModel* ruimodel) : ruimodel_(ruimodel) {

        }

        virtual void OnRobotModeChanged(int handler, RobotMode mode)
        {
            qDebug() << "OnRobotModeChanged";
            qDebug() << "id : " << handler << " , mode : " << static_cast<int>(mode);
        }

         virtual void OnRobotErrorEventReceived(int handler, int error)
        {
            qDebug() << "OnRobotModeChanged";
            qDebug() << "id : " << handler << " , error : " << error;
        }

        virtual void OnRobotConnected(int handler)
        {
            qDebug() << "OnRobotConnected : " << handler;
            // TODO : Handle Connection
            ruimodel_->SetRobotHandler(handler);

        }

        virtual void OnRobotDisconnected(int handler)
        {
            qDebug() << "OnRobotDisconnected : " << handler;
            // TODO : Handle Disconnection
            ruimodel_->SetRobotHandler(-1);
        }

        virtual void OnRobotDebugInfoReceived(int handler, const std::string debug_info, int state)
        {
            qDebug() << "OnRobotDebugInfoReceived : ";
            qDebug() << "id : " << handler << ", debug_info : " << debug_info.c_str() << " , state : " << state;
        }

        virtual void OnRobotInvalidDisconnected(int handler)
        {
            qDebug() << "OnRobotInvalidDisconnected : " << handler;
        }
    };

    std::shared_ptr<ImageReceiveListenerImp> image_receive_listener_impl_;
    std::shared_ptr<RobotEventListenerImp> robot_event_listener_impl_;
};

#endif // RUIMODEL_H
