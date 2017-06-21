#include "ruimodel.h"
#include "ruicontrol.h"
#include <queue>
#include <QDebug>

RUIModel::RUIModel(QObject *parent):
    QObject(parent)
{ 

}


RUIModel::~RUIModel()
{
    if(UdpLocalPort)
        CloseUdpPort(&UdpLocalPort);
}


RUIModel* RUIModel::GetInstance()
{
    static RUIModel m;
    return &m;
}

void RUIModel::Connect()
{
    UdpLocalPort = NULL;
    addrlen = sizeof(remaddr);

    if((UdpLocalPort=OpenUdpPort(1024))==NULL)
    {
        qDebug() << "OpenUdpPort Failed\n";
    }

    //signal slot test
    //CreateSignal();

}


void RUIModel::Disconnect()
{
    if(UdpLocalPort)
        CloseUdpPort(&UdpLocalPort);
}


int RUIModel::GetRobotStatus()
{
    return 0;
}


int RUIModel::GetImage(cv::Mat *Image)
{
    int retvalue = UdpRecvImageAsJpeg(UdpLocalPort,Image,(struct sockaddr *)&remaddr, &addrlen);

    return retvalue;
}


void RUIModel::HandleRobotOperation()
{

}


void RUIModel::SetRobotMode(int mode)
{
    yolo::YMessage msg;
    msg.setOpcode(21);
    msg.set("mode_control", mode);
    //send
}


void RUIModel::HandlePanOperation()
{
    yolo::YMessage msg;
    msg.setOpcode(11);
    msg.set("camera_pan_control", 50);
    //send
}

void RUIModel::HandleTiltOperation()
{
    yolo::YMessage msg;
    msg.setOpcode(12);
    msg.set("camera_tilt_control", 50);
    //send
}

void RUIModel::CreateSignal()
{
    emit RobotStatusChanged(3);
}
