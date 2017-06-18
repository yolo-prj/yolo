#-------------------------------------------------
#
# Project created by QtCreator 2017-06-17T17:33:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YoloExperiments1
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
            ../Common
}

win32 {
    message("WIN32 Compile")
}

macx {
    message("Apple OS Compile")

    INCLUDEPATH += /usr/local/include \
                    ../Common

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
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../Common/NetworkUDP.cpp \
    ../Common/UdpSendRecvJpeg.cpp

HEADERS += \
        mainwindow.h \
    ../Common/NetworkUDP.h \
    ../Common/UdpSendRecvJpeg.h

FORMS += \
        mainwindow.ui
