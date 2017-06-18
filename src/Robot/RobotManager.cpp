#include "RobotManager.h"

RobotManager::RobotManager(YNetworkManager* manager, string msgFormatFile)
{
    _param.resize(2);
    _param[0] = cv::IMWRITE_JPEG_QUALITY;
    _param[1] = 80;

    _networkManager = manager;
    _imageSender = new YImageSender(manager);
    _eventSender = new YEventSender(manager);
    _cmdReceiver = new YCommandReceiver(this, msgFormatFile);
    _configReceiver = new YConfigReceiver(this, msgFormatFile);

    _cameraController = new YCameraController(this);
    _sonarController = new YSonarController(0, 0, this);
    _servoController = new YServoController();


    // fix below for the msg interfaces
    manager->addNetworkMessageListener(1, _cmdReceiver);
    manager->addNetworkMessageListener(2, _configReceiver);
}

RobotManager::~RobotManager()
{
    delete _imageSender;
    delete _eventSender;
    delete _cmdReceiver;
    delete _configReceiver;
}

void
RobotManager::start()
{
    _cameraController->start();
    _servoController->start();
    _sonarController->init(100);
}

void
RobotManager::stop()
{
    _networkManager->stop();
    _cameraController->stop();
    _servoController->stop();
    _sonarController->stop();
}

void
RobotManager::onReceiveImage(Mat image)
{
    uint length = 0;
    byte* data = nullptr;

    data = convertImageToJPEG(image, length);

    // pass image data to image sender object
    YMessage msg;
    msg.setOpcode(10);
    msg.set(data, length);

    _imageSender->send(msg);
}

void
RobotManager::onReceiveDistance(double distanceCm)
{
}

void
RobotManager::onReceiveCommand(YMessage msg)
{
    cout << "[RobotManager] received command" << endl;
}

void
RobotManager::onReceiveConfig(YMessage msg)
{
    cout << "[RobotManager] received config message" << endl;
}

byte*
RobotManager::convertImageToJPEG(Mat image, uint& length)
{
    byte* data;

    cv::imencode(".jpg", image, _buffer, _param);

    data = _buffer.data();
    length = _buffer.size();

    return data;
}
