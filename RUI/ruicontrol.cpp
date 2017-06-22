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
    m->SetRobotMode(RobotMode::MANUAL_MODE);

    QObject::connect(m, SIGNAL(UpdateRobotMode(RobotMode)), this, SLOT(RobotModeHandler(RobotMode)));
}

Controller::~Controller()
{
    m->Disconnect();
    delete ui;
}

void Controller::RobotModeHandler(RobotMode mode)
{
    qDebug() << "signal slot test";
    if(mode == RobotMode::SUSPENDED_MODE)
    {
        QMessageBox msgBox;
        msgBox.setText("Autonomous mode is failed");
        msgBox.setInformativeText("Do you want to change Manual mode?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if(msgBox.exec() == QMessageBox::Yes)
        {
            // change to manual mode
            m->SetRobotMode(RobotMode::MANUAL_MODE);
        }
    }
}

void Controller::timerEvent(QTimerEvent *event)
{
    //cv::Mat image;
    //int retvalue = m->GetImage(&image);

    if(m->IsEmpty())
    {
        qDebug() << "No Data";
        return;
    }

    cv::Mat image = m->GetImage();

    if(image.empty())
    {
        qDebug() << "Invalid Data";
        return;
    }

    ui->openCVviewer->showImage(image);
}


void Controller::on_start_toggled(bool checked)
{
    if(checked)
    {
        m->Connect();
        startTimer(20);
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
    m->HandlePanOperation();
}


void Controller::on_left_pressed()
{
    m->HandleTiltOperation();
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
    m->SetRobotMode(RobotMode::AUTO_MODE);
}

void Controller::on_manual_clicked()
{
    m->SetRobotMode(RobotMode::MANUAL_MODE);
}

void Controller::on_send_clicked()
{
    QString text;
    text = ui->command->text();
    //send
}

