#include "ruicontrol.h"
#include "ui_ruicontrol.h"
#include <QDebug>
#include <QMessageBox>

Controller::Controller(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Controller),
    m(NULL)
{
    ui->setupUi(this);
    m = RUIModel::GetInstance();
    ui->manual->setChecked(true);

    ui->status_green->hide();
    ui->status_red->show();

    QObject::connect(m, SIGNAL(UpdateRobotMode(int)), this, SLOT(RobotModeHandler(int)));
    QObject::connect(m, SIGNAL(UpdateRobotError(int)), this, SLOT(RobotErrorHandler(int)));
    QObject::connect(m, SIGNAL(UpdateRobotConnectionStatus(int)), this, SLOT(RobotConnectionHandler(int)));
    QObject::connect(m, SIGNAL(UpdateRobotDebugInfo(QString)), this, SLOT(RobotDebugInfoHandler(QString)));
    QObject::connect(m, SIGNAL(UpdateRobotInvalidDisconnection()), this, SLOT(RobotInvalidDisconnectionHandler()));

}

Controller::~Controller()
{
    delete ui;
}

// Robot -> RUI
void Controller::RobotModeHandler(int mode)
{
    qDebug() << "robot mode is changed";

    switch(mode)
    {
        case 1: //RobotMode::AUTO_MODE
            ui->auto_2->setChecked(true);
            ui->manual->setChecked(false);
            break;
        case 2: //RobotMode::MANUAL_MODE:
            ui->manual->setChecked(true);
            ui->auto_2->setChecked(false);
            m->HandlePanOperation(ui->horizontalSlider->sliderPosition());
            m->HandleTiltOperation(ui->horizontalSlider_2->sliderPosition());
            break;
        case 0: //RobotMode::IDLE_MODE:
        case 3: //RobotMode::SUSPENDED_MODE:
        default:
            ui->auto_2->setChecked(false);
            ui->manual->setChecked(false);
            break;
    }
}

// error
void Controller::RobotErrorHandler(int error)
{
    QMessageBox msgBox;
    QString text;

    msgBox.setText("Autonomous mode is failed!!! Do you want to change Manual mode? ");

    switch(error)
    {
        case 1:
            text = "cause: robot cannot find red dot or road sign.";
            break;
        case 2:
            text = "cause: robot cannot recognize a road sign";
            break;
        case 3:
            text = "cause: robot is out of track";
            break;
        default:
            break;
    }

    msgBox.setInformativeText(text);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::Yes)
    {
        // change to manual mode
        m->SetRobotMode(RobotMode::MANUAL_MODE);
        ui->manual->setChecked(true);
        ui->auto_2->setChecked(false);
    }
}

// connect, disconnect
void Controller::RobotConnectionHandler(int status)
{
    qDebug () << "Robot connection status" << status;

    QMessageBox msgBox;
    QString text;

    if(status) // connected
    {
        ui->status_green->show();
        ui->status_red->hide();
        text = "Robot is conneted!!!";
    }
    else // disconnected
    {

        ui->status_green->hide();
        ui->status_red->show();
        text = "Robot is disconnected!!!";
    }
    msgBox.setText(text);
    msgBox.exec();
}

// debug info
void Controller::RobotDebugInfoHandler(QString diag)
{
    ui->diag->setPlainText(diag);

}

// invalid disconnection
void Controller::RobotInvalidDisconnectionHandler()
{
    QMessageBox msgBox;

    msgBox.setText("Network connection is lost!!!");
    msgBox.setInformativeText("Please wait for connection re-establishment.");
    msgBox.exec();
}


// image
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
    static int id = 0;

    if(checked)
    {
        m->SetImageOnOff(1);
        id = startTimer(20);
    }
    else
    {
        m->SetImageOnOff(0);
        killTimer(id);
    }
}

// robot operation
void Controller::on_go_pressed()
{
    if(ui->manual->isChecked())
        m->HandleRobotForwardOperation(PRESS);
}

void Controller::on_go_released()
{
    if(ui->manual->isChecked())
        m->HandleRobotForwardOperation(RELEASE);
}


void Controller::on_back_pressed()
{
    if(ui->manual->isChecked())
        m->HandleRobotBackwardOperation(PRESS);
}

void Controller::on_back_released()
{
    if(ui->manual->isChecked())
        m->HandleRobotBackwardOperation(RELEASE);
}


void Controller::on_right_pressed()
{
    if(ui->manual->isChecked())
        m->HandleRobotRightOperation(PRESS);
}

void Controller::on_right_released()
{
    if(ui->manual->isChecked())
        m->HandleRobotRightOperation(RELEASE);
}


void Controller::on_left_pressed()
{
    if(ui->manual->isChecked())
        m->HandleRobotLeftOperation(PRESS);
}

void Controller::on_left_released()
{
    if(ui->manual->isChecked())
        m->HandleRobotLeftOperation(RELEASE);
}


void Controller::on_uturn_clicked()
{
    if(ui->manual->isChecked())
        m->HandleRobotUturnOperation();
}


// camera
void Controller::on_horizontalSlider_sliderMoved(int position)
{
    if(ui->manual->isChecked())
    {
        m->HandlePanOperation(position);
        m->HandleTiltOperation(ui->horizontalSlider_2->sliderPosition());
    }
}


void Controller::on_horizontalSlider_2_sliderMoved(int position)
{
    if(ui->manual->isChecked())
    {
        m->HandleTiltOperation(position);
        m->HandlePanOperation(ui->horizontalSlider->sliderPosition());
    }
}


// mode control
void Controller::on_auto_2_clicked(bool checked)
{
    if(checked)
        m->SetRobotMode(RobotMode::AUTO_MODE);
}

void Controller::on_manual_clicked(bool checked)
{
    if(checked)
        m->SetRobotMode(RobotMode::MANUAL_MODE);
}


void Controller::on_send_clicked()
{
    QString text;
    text = ui->command->text();

    m->SendTextInput(text);
}



