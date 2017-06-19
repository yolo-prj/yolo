CC=g++
DEFINE=RUI
#DEFINE=ROBOT
ifeq ($(DEFINE), ROBOT)
INCLUDE_RASPICAM=-I/home/pi/git/robidouille/raspicam_cv
LIB_RASPICAM=-L/home/pi/git/robidouille/raspicam_cv
else
INCLUDE_RASPICAM=
LIB_RASPICAM=
endif
INCLUDE=-I/usr/local/include -I../Common/include $(INCLUDE_RASPICAM)
LIBRARY=$(LIB_RASPICAM)
HEADERDIR=../Common/include/yolo/YFramework
LIBDIR=../Common/lib
BIN=Robot
OPTION=-std=c++11
OBJECT=YMessageSender.o YMessageReceiver.o RobotManager.o PID.o ImageProcessor.o
BOOSTLIB=-lboost_system -lboost_thread -lpthread
ifeq ($(DEFINE), ROBOT)
LINK_RASPICAM=-lraspicamcv
else
LINK_RASPICAM=
endif
LINK_OPENCV=-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio
LINK_LIB=$(BOOSTLIB) -lYComm -lYFramework $(LINK_OPENCV) $(LINK_RASPICAM)

all: $(OBJECT) $(BIN).cpp
	$(CC) $(OPTION) -o $(BIN) $(INCLUDE) $(LIBRARY) $(BIN).cpp $(OBJECT) $(LINK_LIB)
YMessageSender.o: YMessageSender.h YMessageSender.cpp
	$(CC) $(OPTION) -c $(INCLUDE) YMessageSender.cpp
YMessageReceiver.o: YMessageReceiver.h YMessageReceiver.cpp
	$(CC) $(OPTION) -c $(INCLUDE) YMessageReceiver.cpp
RobotManager.o: RobotManager.h RobotManager.cpp
	$(CC) $(OPTION) -c $(INCLUDE) RobotManager.cpp
PID.o: PID.h PID.cpp
	$(CC) $(OPTION) -c $(INCLUDE) PID.cpp
ImageProcessor.o: ImageProcessor.h ImageProcessor.cpp
	$(CC) $(OPTION) -c $(INCLUDE) ImageProcessor.cpp

clean:
	rm *.o
	rm $(BIN)
