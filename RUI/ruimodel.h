#ifndef RUIMODEL_H
#define RUIMODEL_H

#include "NetworkUDP.h"
#include "UdpSendRecvJpeg.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "YMessage.h"
#include <QString>
#include <QObject>


#define AUTO_MODE 1
#define MANUAL_MODE 2
#define SUSPEND_MODE 3


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

    virtual void SetRobotMode(int mode);

    virtual void HandlePanOperation();
    virtual void HandleTiltOperation();

    // signal slot test
    void CreateSignal();

private:
    TUdpLocalPort *UdpLocalPort;
    struct sockaddr_in remaddr;
    socklen_t addrlen;

};

#endif // RUIMODEL_H
