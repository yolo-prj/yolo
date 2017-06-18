#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTimer>

#include "NetworkUDP.h"
#include "UdpSendRecvJpeg.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots :
    void on_btnPause_clicked();

private:
    Ui::MainWindow *ui;

    // OpenCV class
    cv::VideoCapture capture;
    cv::Mat original;

    // QImage class
    QImage qimgOriginal;

    // QTimer class - to give time to respond to UI
    QTimer *tmrTimer;

    TUdpLocalPort *UdpLocalPort;
    struct sockaddr_in remaddr;	/* remote address */
    socklen_t addrlen;

public slots:
    void updateFrame();
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOW_H
