#-------------------------------------------------
#
# Project created by QtCreator 2017-06-22T04:32:11
#
#-------------------------------------------------
CONFIG += c++14
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

linux-g++ | linux-g++-64 | linux-g++-32 {
message("Linux Compile")

LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio

INCLUDEPATH += /usr/local/include \
            ../Common \
	    Test \
            ../Common/include

    LIBS += -lboost_system \
            -lboost_thread \
            -lboost_exception \
            -lboost_regex \
            -lboost_timer
}

win32 {
    message("WIN32 Compile")
}

macx {
    message("Apple OS Compile")

    INCLUDEPATH += /usr/local/include \
                   ../Common/include

    LIBS += -L/usr/local/lib \
            -lopencv_calib3d \
            -lopencv_contrib \
            -lopencv_core \
            -lopencv_features2d \
            -lopencv_flann \
            -lopencv_gpu \
            -lopencv_highgui \
            -lopencv_imgproc \
            -lopencv_legacy \
            -lopencv_ml \
            -lopencv_nonfree \
            -lopencv_objdetect \
            -lopencv_ocl \
            -lopencv_photo \
            -lopencv_stitching \
            -lopencv_superres \
            -lopencv_ts \
            -lopencv_video \
            -lopencv_videostab

    LIBS += -lboost_system \
            -lboost_thread-mt \
            -lboost_exception \
            -lboost_regex \
            -lboost_timer
}

SOURCES += main.cpp\
        ruimodel.cpp \
        cqtopencvviewergl.cpp \
    	ruicontrol.cpp \
        ../Common/src/YComm/YComm.cpp \
        ../Common/src/YComm/YTcpServer.cpp \
        ../Common/src/YComm/YTcpSession.cpp \
        ../Common/src/YComm/YUdp.cpp \
        ../Common/src/YComm/YUdpReceiveDatagramHandler.cpp \
        ../Common/src/YFramework/YHeartbeatManager.cpp \
        ../Common/src/YFramework/YMessage.cpp \
        ../Common/src/YFramework/YMessageFormatParser.cpp \
        ../Common/src/YFramework/YNetworkManager.cpp \
        ../Common/src/YComm/YTcpClient.cpp \
    	robotcontrolmanager.cpp \
	robotcontroller.cpp \
	imagereceiver.cpp 	

HEADERS  += \
        ruimodel.h \
        cqtopencvviewergl.h \
        ruicontrol.h \
        ../Common/include/yolo/YFramework/YCameraImageListener.h \
        ../Common/include/yolo/YFramework/YConnectionLostListener.h \
        ../Common/include/yolo/YFramework/YHeartbeatManager.h \
        ../Common/include/yolo/YFramework/YMessage.h \
        ../Common/include/yolo/YFramework/YMessageFormatParser.h \
        ../Common/include/yolo/YFramework/YNetworkManager.h \
        ../Common/include/yolo/YFramework/YNetworkMessageListener.h \
        ../Common/include/yolo/YComm/YComm.h \
        ../Common/include/yolo/YComm/YCommCommon.h \
        ../Common/include/yolo/YComm/YCommMain.h \
        ../Common/include/yolo/YComm/YTcpConnectionEvent.h \
        ../Common/include/yolo/YComm/YTcpReceiveDataEvent.h \
        ../Common/include/yolo/YComm/YTcpServer.h \
        ../Common/include/yolo/YComm/YTcpSession.h \
        ../Common/include/yolo/YComm/YUdp.h \
        ../Common/include/yolo/YComm/YUdpReceiveDatagramEvent.h \
        ../Common/include/yolo/YComm/YUdpReceiveDatagramHandler.h \
        robotcontrolmanager.h \
        robotcontroller.h \
        roboteventlistener.h \
        imagereceivelistener.h \
        robotmode.h \
        templatesingletone.h \
    imagereceiver.h \
    robotmovement.h \
    robotmovement.h

FORMS    += \
         ruicontrol.ui

DISTFILES += \
    assets/bullet_green.png \
    assets/bullet_red.png \
    assets/bullet_yellow.png

RESOURCES += \
    res.qrc

