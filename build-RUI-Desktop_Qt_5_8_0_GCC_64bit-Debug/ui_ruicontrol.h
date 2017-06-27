/********************************************************************************
** Form generated from reading UI file 'ruicontrol.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RUICONTROL_H
#define UI_RUICONTROL_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <cqtopencvviewergl.h>

QT_BEGIN_NAMESPACE

class Ui_Controller
{
public:
    QAction *actionQuit;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QTabWidget *tab_manager;
    QWidget *Robot1;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *Camera;
    QVBoxLayout *verticalLayout;
    CQtOpenCVViewerGl *openCVviewer;
    QWidget *Robot2;
    QGroupBox *Camera_2;
    QVBoxLayout *verticalLayout_8;
    CQtOpenCVViewerGl *openCVviewer_2;
    QDockWidget *dock_status;
    QWidget *dockWidgetContents_4;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_5;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox_4;
    QPushButton *go;
    QPushButton *back;
    QPushButton *left;
    QPushButton *right;
    QPushButton *uturn;
    QSplitter *splitter;
    QLabel *label_4;
    QRadioButton *auto_2;
    QRadioButton *manual;
    QLabel *status_red;
    QLabel *status_green;
    QLabel *label_3;
    QFrame *frame_6;
    QGroupBox *groupBox_5;
    QLabel *label_5;
    QLabel *label_6;
    QSplitter *splitter_2;
    QSlider *horizontalSlider;
    QSlider *horizontalSlider_2;
    QSplitter *splitter_3;
    QLabel *label_2;
    QRadioButton *start;
    QRadioButton *stop;
    QFrame *frame;
    QGroupBox *groupBox_6;
    QLabel *label;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_3;
    QPlainTextEdit *diag;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *command;
    QPushButton *send;
    QMenuBar *menuBar;
    QMenu *menuMenu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Controller)
    {
        if (Controller->objectName().isEmpty())
            Controller->setObjectName(QStringLiteral("Controller"));
        Controller->resize(989, 650);
        actionQuit = new QAction(Controller);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        centralWidget = new QWidget(Controller);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tab_manager = new QTabWidget(centralWidget);
        tab_manager->setObjectName(QStringLiteral("tab_manager"));
        tab_manager->setEnabled(true);
        tab_manager->setMinimumSize(QSize(100, 0));
        tab_manager->setLocale(QLocale(QLocale::English, QLocale::Australia));
        Robot1 = new QWidget();
        Robot1->setObjectName(QStringLiteral("Robot1"));
        verticalLayout_4 = new QVBoxLayout(Robot1);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        Camera = new QGroupBox(Robot1);
        Camera->setObjectName(QStringLiteral("Camera"));
        Camera->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Camera->sizePolicy().hasHeightForWidth());
        Camera->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(Camera);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        openCVviewer = new CQtOpenCVViewerGl(Camera);
        openCVviewer->setObjectName(QStringLiteral("openCVviewer"));

        verticalLayout->addWidget(openCVviewer);


        verticalLayout_4->addWidget(Camera);

        tab_manager->addTab(Robot1, QString());
        Robot2 = new QWidget();
        Robot2->setObjectName(QStringLiteral("Robot2"));
        Camera_2 = new QGroupBox(Robot2);
        Camera_2->setObjectName(QStringLiteral("Camera_2"));
        Camera_2->setGeometry(QRect(10, 10, 453, 522));
        sizePolicy.setHeightForWidth(Camera_2->sizePolicy().hasHeightForWidth());
        Camera_2->setSizePolicy(sizePolicy);
        verticalLayout_8 = new QVBoxLayout(Camera_2);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        openCVviewer_2 = new CQtOpenCVViewerGl(Camera_2);
        openCVviewer_2->setObjectName(QStringLiteral("openCVviewer_2"));

        verticalLayout_8->addWidget(openCVviewer_2);

        tab_manager->addTab(Robot2, QString());

        gridLayout->addWidget(tab_manager, 0, 0, 1, 1);

        dock_status = new QDockWidget(centralWidget);
        dock_status->setObjectName(QStringLiteral("dock_status"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(dock_status->sizePolicy().hasHeightForWidth());
        dock_status->setSizePolicy(sizePolicy1);
        dock_status->setMinimumSize(QSize(372, 571));
        dock_status->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dock_status->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_4 = new QWidget();
        dockWidgetContents_4->setObjectName(QStringLiteral("dockWidgetContents_4"));
        verticalLayout_2 = new QVBoxLayout(dockWidgetContents_4);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        frame_5 = new QFrame(dockWidgetContents_4);
        frame_5->setObjectName(QStringLiteral("frame_5"));
        sizePolicy1.setHeightForWidth(frame_5->sizePolicy().hasHeightForWidth());
        frame_5->setSizePolicy(sizePolicy1);
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame_5);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupBox_4 = new QGroupBox(frame_5);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        go = new QPushButton(groupBox_4);
        go->setObjectName(QStringLiteral("go"));
        go->setGeometry(QRect(170, 34, 89, 25));
        back = new QPushButton(groupBox_4);
        back->setObjectName(QStringLiteral("back"));
        back->setGeometry(QRect(170, 94, 89, 25));
        left = new QPushButton(groupBox_4);
        left->setObjectName(QStringLiteral("left"));
        left->setGeometry(QRect(76, 64, 89, 25));
        right = new QPushButton(groupBox_4);
        right->setObjectName(QStringLiteral("right"));
        right->setGeometry(QRect(264, 64, 89, 25));
        uturn = new QPushButton(groupBox_4);
        uturn->setObjectName(QStringLiteral("uturn"));
        uturn->setGeometry(QRect(170, 64, 89, 25));
        splitter = new QSplitter(groupBox_4);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(10, 112, 321, 51));
        splitter->setOrientation(Qt::Horizontal);
        label_4 = new QLabel(splitter);
        label_4->setObjectName(QStringLiteral("label_4"));
        splitter->addWidget(label_4);
        auto_2 = new QRadioButton(splitter);
        auto_2->setObjectName(QStringLiteral("auto_2"));
        splitter->addWidget(auto_2);
        manual = new QRadioButton(splitter);
        manual->setObjectName(QStringLiteral("manual"));
        splitter->addWidget(manual);
        status_red = new QLabel(groupBox_4);
        status_red->setObjectName(QStringLiteral("status_red"));
        status_red->setGeometry(QRect(389, 2, 31, 17));
        status_red->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/assets/bullet_red.png")));
        status_green = new QLabel(groupBox_4);
        status_green->setObjectName(QStringLiteral("status_green"));
        status_green->setGeometry(QRect(366, 2, 31, 17));
        status_green->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/assets/bullet_green.png")));
        label_3 = new QLabel(groupBox_4);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(300, 1, 81, 20));
        go->raise();
        back->raise();
        left->raise();
        right->raise();
        uturn->raise();
        splitter->raise();
        status_red->raise();
        status_green->raise();
        tab_manager->raise();
        label_3->raise();

        verticalLayout_3->addWidget(groupBox_4);


        verticalLayout_2->addWidget(frame_5);

        frame_6 = new QFrame(dockWidgetContents_4);
        frame_6->setObjectName(QStringLiteral("frame_6"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(frame_6->sizePolicy().hasHeightForWidth());
        frame_6->setSizePolicy(sizePolicy2);
        frame_6->setLayoutDirection(Qt::LeftToRight);
        frame_6->setFrameShape(QFrame::StyledPanel);
        frame_6->setFrameShadow(QFrame::Raised);
        frame_6->setLineWidth(1);
        groupBox_5 = new QGroupBox(frame_6);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(9, 9, 441, 161));
        label_5 = new QLabel(groupBox_5);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 80, 31, 21));
        label_5->setLayoutDirection(Qt::LeftToRight);
        label_6 = new QLabel(groupBox_5);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 120, 31, 21));
        label_6->setLayoutDirection(Qt::LeftToRight);
        splitter_2 = new QSplitter(groupBox_5);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setGeometry(QRect(50, 70, 371, 71));
        splitter_2->setOrientation(Qt::Vertical);
        horizontalSlider = new QSlider(splitter_2);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setMinimum(50);
        horizontalSlider->setMaximum(250);
        horizontalSlider->setValue(150);
        horizontalSlider->setSliderPosition(150);
        horizontalSlider->setOrientation(Qt::Horizontal);
        horizontalSlider->setInvertedAppearance(true);
        horizontalSlider->setInvertedControls(true);
        horizontalSlider->setTickPosition(QSlider::TicksAbove);
        horizontalSlider->setTickInterval(50);
        splitter_2->addWidget(horizontalSlider);
        horizontalSlider_2 = new QSlider(splitter_2);
        horizontalSlider_2->setObjectName(QStringLiteral("horizontalSlider_2"));
        horizontalSlider_2->setMinimum(80);
        horizontalSlider_2->setMaximum(220);
        horizontalSlider_2->setValue(150);
        horizontalSlider_2->setSliderPosition(150);
        horizontalSlider_2->setOrientation(Qt::Horizontal);
        horizontalSlider_2->setInvertedAppearance(true);
        horizontalSlider_2->setTickPosition(QSlider::TicksAbove);
        horizontalSlider_2->setTickInterval(50);
        splitter_2->addWidget(horizontalSlider_2);
        splitter_3 = new QSplitter(groupBox_5);
        splitter_3->setObjectName(QStringLiteral("splitter_3"));
        splitter_3->setGeometry(QRect(10, 40, 251, 23));
        splitter_3->setOrientation(Qt::Horizontal);
        label_2 = new QLabel(splitter_3);
        label_2->setObjectName(QStringLiteral("label_2"));
        splitter_3->addWidget(label_2);
        start = new QRadioButton(splitter_3);
        start->setObjectName(QStringLiteral("start"));
        splitter_3->addWidget(start);
        stop = new QRadioButton(splitter_3);
        stop->setObjectName(QStringLiteral("stop"));
        splitter_3->addWidget(stop);

        verticalLayout_2->addWidget(frame_6);

        frame = new QFrame(dockWidgetContents_4);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        groupBox_6 = new QGroupBox(frame);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 10, 441, 161));
        label = new QLabel(groupBox_6);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(13, 105, 91, 17));
        layoutWidget = new QWidget(groupBox_6);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(13, 30, 421, 72));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        diag = new QPlainTextEdit(layoutWidget);
        diag->setObjectName(QStringLiteral("diag"));
        diag->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        horizontalLayout_3->addWidget(diag);

        layoutWidget1 = new QWidget(groupBox_6);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(13, 127, 421, 27));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        command = new QLineEdit(layoutWidget1);
        command->setObjectName(QStringLiteral("command"));

        horizontalLayout_2->addWidget(command);

        send = new QPushButton(layoutWidget1);
        send->setObjectName(QStringLiteral("send"));

        horizontalLayout_2->addWidget(send);


        verticalLayout_2->addWidget(frame);

        dock_status->setWidget(dockWidgetContents_4);

        gridLayout->addWidget(dock_status, 0, 1, 1, 1);

        Controller->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Controller);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 989, 22));
        menuMenu = new QMenu(menuBar);
        menuMenu->setObjectName(QStringLiteral("menuMenu"));
        Controller->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Controller);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Controller->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Controller);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Controller->setStatusBar(statusBar);

        menuBar->addAction(menuMenu->menuAction());
        menuMenu->addAction(actionQuit);

        retranslateUi(Controller);
        QObject::connect(actionQuit, SIGNAL(triggered(bool)), Controller, SLOT(close()));

        tab_manager->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Controller);
    } // setupUi

    void retranslateUi(QMainWindow *Controller)
    {
        Controller->setWindowTitle(QApplication::translate("Controller", "Controller", Q_NULLPTR));
        actionQuit->setText(QApplication::translate("Controller", "Quit", Q_NULLPTR));
        Camera->setTitle(QApplication::translate("Controller", "Camera", Q_NULLPTR));
        tab_manager->setTabText(tab_manager->indexOf(Robot1), QApplication::translate("Controller", "Robot1", Q_NULLPTR));
        Camera_2->setTitle(QApplication::translate("Controller", "Camera", Q_NULLPTR));
        tab_manager->setTabText(tab_manager->indexOf(Robot2), QApplication::translate("Controller", "Robot2", Q_NULLPTR));
        dock_status->setWindowTitle(QApplication::translate("Controller", "Command Panel", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("Controller", "Robot Control", Q_NULLPTR));
        go->setText(QApplication::translate("Controller", "Go", Q_NULLPTR));
        back->setText(QApplication::translate("Controller", "Back", Q_NULLPTR));
        left->setText(QApplication::translate("Controller", "Left Turn", Q_NULLPTR));
        right->setText(QApplication::translate("Controller", "Right Turn", Q_NULLPTR));
        uturn->setText(QApplication::translate("Controller", "U Turn", Q_NULLPTR));
        label_4->setText(QApplication::translate("Controller", "Operation Mode", Q_NULLPTR));
        auto_2->setText(QApplication::translate("Controller", "Autonomous", Q_NULLPTR));
        manual->setText(QApplication::translate("Controller", "Manual", Q_NULLPTR));
        status_red->setText(QString());
#ifndef QT_NO_TOOLTIP
        status_green->setToolTip(QApplication::translate("Controller", "<html><head/><body><p>Robot connection</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        status_green->setText(QString());
        label_3->setText(QApplication::translate("Controller", "<html><head/><body><p><span style=\" font-size:10pt;\">Connection</span></p></body></html>", Q_NULLPTR));
        groupBox_5->setTitle(QApplication::translate("Controller", "Camera Control", Q_NULLPTR));
        label_5->setText(QApplication::translate("Controller", "<html><head/><body><p>Pan</p></body></html>", Q_NULLPTR));
        label_6->setText(QApplication::translate("Controller", "<html><head/><body><p>Tilt</p></body></html>", Q_NULLPTR));
        label_2->setText(QApplication::translate("Controller", "Image Streaming", Q_NULLPTR));
        start->setText(QApplication::translate("Controller", "Play", Q_NULLPTR));
        stop->setText(QApplication::translate("Controller", "Pause", Q_NULLPTR));
        groupBox_6->setTitle(QApplication::translate("Controller", "Diagnostic Control", Q_NULLPTR));
        label->setText(QApplication::translate("Controller", "Command", Q_NULLPTR));
        send->setText(QApplication::translate("Controller", "Send", Q_NULLPTR));
        menuMenu->setTitle(QApplication::translate("Controller", "Menu", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Controller: public Ui_Controller {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RUICONTROL_H
