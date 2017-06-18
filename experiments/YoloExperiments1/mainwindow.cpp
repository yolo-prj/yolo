#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>

using namespace std;
using namespace cv;


#define WIDTH 320
#define HEIGHT 240

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    UdpLocalPort = NULL;
    addrlen = sizeof(remaddr);/* length of addresses */

    if  ((UdpLocalPort=OpenUdpPort(9001))==NULL)  // Open UDP Network port
    {
        qDebug() << "OpenUdpPort Failed\n";
    }

    // Set initial time
    ui->txtLoopTime->appendPlainText(QString("20"));

    // Timer for UI responsivness
    tmrTimer = new QTimer(this);
    connect(tmrTimer,SIGNAL(timeout()),this,SLOT(updateFrame()));
    tmrTimer->start(ui->txtLoopTime->toPlainText().toInt()); //ui->txtLoopTime->toPlainText().toInt() msec
}

MainWindow::~MainWindow()
{
    CloseUdpPort(&UdpLocalPort); // Close network port;
    delete ui;
}

void MainWindow::updateFrame() {

    //if(original.empty() == true)
    //    return;

    Size size(320*2,240*2);
    Mat image;
    int retvalue = UdpRecvImageAsJpeg(UdpLocalPort,&image,(struct sockaddr *)&remaddr, &addrlen);

    if(!retvalue)
    {
        qDebug() << "No Data\n";
        return;
    }
    cv::resize(image,original,size);
    // OpenCV to QImage datatype to display on labels
    cv::cvtColor(original,original,CV_BGR2RGB);
    QImage qimgOriginal((uchar*) original.data,original.cols,original.rows,original.step,QImage::Format_RGB888); // for color images
    //QImage qimgProcessed((uchar*) processed.data,processed.cols,processed.rows,processed.step,QImage::Format_Indexed8); // for grayscale images

    // Update the labels on the form
    ui->lblDisplay->setPixmap(QPixmap::fromImage(qimgOriginal));
    //ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));
}

// Button for pause stream
void MainWindow::on_btnPause_clicked()
{
    if(tmrTimer->isActive() == true){ // timer is running
        tmrTimer->stop();
        ui->btnPause->setText("resume");
    }
    else{
        tmrTimer->start(ui->txtLoopTime->toPlainText().toInt());
        ui->btnPause->setText("pause");
    }
}

// Handle event filter (enter button press)
bool MainWindow::eventFilter(QObject *object, QEvent *event){
    return QMainWindow::eventFilter(object, event);
}
