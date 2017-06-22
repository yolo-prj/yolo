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
    m->SetRobotMode(RobotMode::MANUAL_MODE);

    QObject::connect(m, SIGNAL(UpdateRobotMode(RobotMode)), this, SLOT(RobotModeHandler(RobotMode)));
    QObject::connect(m, SIGNAL(UpdateRobotError(int)), this, SLOT(RobotErrorHandler(int)));
    QObject::connect(m, SIGNAL(UpdateRobotConnectionStatus(int)), this, SLOT(RobotConnectionHandler(int)));
    QObject::connect(m, SIGNAL(UpdateRobotDebugInfo(QString)), this, SLOT(RobotDebugInfoHandler(QString)));
    QObject::connect(m, SIGNAL(UpdateRobotInvalidDisconnection()), this, SLOT(RobotInvalidDisconnectionHandler()));

}

Controller::~Controller()
{
    m->Disconnect();
    delete ui;
}

// Robot -> RUI
// mode_changed
void Controller::RobotModeHandler(RobotMode mode)
{
    qDebug() << "robot mode is changed";
    // update robot mode to RUI
}

// error
void Controller::RobotErrorHandler(int error)
{
    QMessageBox msgBox;

    msgBox.setText("Autonomous mode is failed");
    // 1 : Adirectional dot or road sign is not present
    // 2 : robot cannot recognize a sign
    // 3 : lost track
    // TODO - change Text according to error

    msgBox.setInformativeText("Do you want to change Manual mode?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::Yes)
    {
        // change to manual mode
        m->SetRobotMode(RobotMode::MANUAL_MODE);
    }
}

// connect, disconnect
void Controller::RobotConnectionHandler(int status)
{
    // update robot connection status to RUI
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

    msgBox.setText("Network connection is lost");
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
    //TODO: change image start/stop
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

// robot operation
void Controller::on_go_pressed()
{
    m->HandleRobotForwardOperation(PRESS);
}

void Controller::on_go_released()
{
    m->HandleRobotForwardOperation(RELEASE);
}


void Controller::on_back_pressed()
{
    m->HandleRobotBackwardOperation(PRESS);
}

void Controller::on_back_released()
{
    m->HandleRobotBackwardOperation(RELEASE);
}


void Controller::on_right_pressed()
{
    m->HandleRobotRightOperation(PRESS);
}

void Controller::on_right_released()
{
    m->HandleRobotRightOperation(RELEASE);
}


void Controller::on_left_pressed()
{
    m->HandleRobotLeftOperation(PRESS);
}

void Controller::on_left_released()
{
    m->HandleRobotLeftOperation(RELEASE);
}


void Controller::on_uturn_clicked()
{
    m->HandleRobotUturnOperation();
}


// camera
void Controller::on_horizontalSlider_sliderMoved()
{
    m->HandlePanOperation();
}


void Controller::on_verticalSlider_sliderMoved()
{
    m->HandleTiltOperation();
}


// mode control
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



