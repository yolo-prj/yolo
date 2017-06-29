#include "ruimodel.h"
#include "ruicontrol.h"

#include "imagereceiver.h"
#include "robotcontrolmanager.h"

RUIModel::RUIModel(QObject *parent):
    QObject(parent),
    cur_robot_handle_(-1)
{ 
    image_receive_listener_impl_ = std::make_shared<ImageReceiveListenerImp>(this);
    robot_event_listener_impl_ = std::make_shared<RobotEventListenerImp>(this);

    yolo::ImageReceiver::GetInstance().RegisterListener(image_receive_listener_impl_);
    yolo::RobotControlManager::GetInstance().RegisterListener(robot_event_listener_impl_);
}


RUIModel::~RUIModel()
{
    while(!image_queue_.empty()) {
        image_queue_.pop();
    }

    yolo::ImageReceiver::GetInstance().UnregisterListener();
    yolo::RobotControlManager::GetInstance().UnregisterListener();
}


RUIModel* RUIModel::GetInstance()
{
    static RUIModel m;
    return &m;
}

// robot operation
void RUIModel::HandleRobotForwardOperation(int operation)
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "servo_control_forward";
    int state = operation;

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         1,
                                                         std::make_tuple(id, command, state));
}

void RUIModel::HandleRobotBackwardOperation(int operation)
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "servo_control_backward";
    int state = operation;

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         2,
                                                         std::make_tuple(id, command, state));
}

void RUIModel::HandleRobotLeftOperation(int operation)
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "servo_control_left";
    int state = operation;

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         3,
                                                         std::make_tuple(id, command, state));
}

void RUIModel::HandleRobotRightOperation(int operation)
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "servo_control_right";
    int state = operation;

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         4,
                                                         std::make_tuple(id, command, state));
}

void RUIModel::HandleRobotUturnOperation()
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "servo_control_uturn";
    int state = 0;

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         5,
                                                         std::make_tuple(id, command, state));
}

// camera
void RUIModel::HandlePanOperation(int position)
{

    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "camera_pan_control";
    int state = position;

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         11,
                                                         std::make_tuple(id, command, state));
}

void RUIModel::HandleTiltOperation(int position)
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "camera_tilt_control";
    int state = position;

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         12,
                                                         std::make_tuple(id, command, state));
}

// mode selection
void RUIModel::SetRobotMode(RobotMode mode)
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "mode_control";
    int state = static_cast<int>(mode);

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         21,
                                                         std::make_tuple(id, command, state));
}

// image
void RUIModel::SetImageOnOff(int operation)
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string command = "image_streaming_control";
    int state = operation;

    yolo::RobotControlManager::GetInstance().SendCommand(cur_robot_handle_,
                                                         31,
                                                         std::make_tuple(id, command, state));
}

bool RUIModel::IsEmpty() const
{
    return image_queue_.empty();
}

int RUIModel::PushImage(cv::Mat &&image)
{
    std::lock_guard<std::mutex> lock(image_mutex_);
    //qDebug() << "PushImage() : " << image_queue_.size();
    image_queue_.push(std::move(image));

    //main_window_->imageUpdated();

    return 0;
}

cv::Mat RUIModel::GetImage()
{
    std::lock_guard<std::mutex> lock(image_mutex_);
    //qDebug() << "GetImage() : " << image_queue_.size();

    if(image_queue_.size() == 0)
    {
        qDebug() << "GetImage() : No more data";
        return cv::Mat();
    }

    cv::Mat image = std::move(image_queue_.front());
    image_queue_.pop();

    return image;
}

// input text
void RUIModel::SendTextInput(QString string)
{
    std::string id = std::to_string(cur_robot_handle_);
    std::string debug = string.toStdString();
    int state = 0;

    yolo::RobotControlManager::GetInstance().SendDebugCommand(cur_robot_handle_,
                                                         debug,
                                                         state);
}




