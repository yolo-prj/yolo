#include "ruicontrol.h"
#include "ui_ruiview.h"
#include <QDebug>
#include <QMessageBox>

Controller::Controller(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Controller),
    m(NULL)
{
    ui->setupUi(this);
    m = RUIModel::GetInstance();
    m->SetRobotMode(MANUAL_MODE);

    QObject::connect(m, SIGNAL(RobotStatusChanged(int)), this, SLOT(RobotStatusHandler(int)));
}

Controller::~Controller()
{
    m->Disconnect();
    delete ui;
}

void Controller::RobotStatusHandler(int a)
{

    qDebug() << "signal slot test";
    if(a == SUSPEND_MODE)
    {
        QMessageBox msgBox;
        msgBox.setText("SUSPEND MODE!!!!");
        msgBox.exec();
    }
}

void Controller::timerEvent(QTimerEvent *event)
{
    cv::Mat image;

    int retvalue = m->GetImage(&image);

    if(!retvalue)
    {
        qDebug() << "No Data";
        return;
    }

    ui->openCVviewer->showImage(image);
}


void Controller::on_start_toggled(bool checked)
{
    if(checked)
    {
        m->Connect();
        startTimer(0);
    }
    else
    {
        m->Disconnect();
    }
}


void Controller::on_go_pressed()
{
    m->HandleRobotOperation();
}


void Controller::on_back_pressed()
{

}


void Controller::on_right_pressed()
{

}


void Controller::on_left_pressed()
{

}

void Controller::on_uturn_clicked()
{

}

void Controller::on_horizontalSlider_sliderMoved()
{
    m->HandlePanOperation();
}


void Controller::on_verticalSlider_sliderMoved()
{
    m->HandleTiltOperation();
}

void Controller::on_auto_2_clicked()
{
    m->SetRobotMode(AUTO_MODE);
}

void Controller::on_manual_clicked()
{
    m->SetRobotMode(MANUAL_MODE);
}

void Controller::on_send_clicked()
{
    QString text;
    text = ui->command->text();
    //send
}

