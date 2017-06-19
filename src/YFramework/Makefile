CC=g++
#DEFINE=ROBOT
DEFINE=RUI
ifeq ($(DEFINE), ROBOT)
INCLUDE_RASPICAM=-I/home/pi/git/robidouille/raspicam_cv
else
INCLUDE_RASPICAM=
endif
INCLUDE=-I/usr/local/include -I../../include $(INCLUDE_RASPICAM)
ifeq ($(DEFINE), ROBOT)
LIBRARY_RASPICAM=-L/home/pi/git/robidouille/raspicam_cv
LINK_RASPICAM=-lraspicamcv
else
LIBRARY_RASPICAM=
LINK_RASPICAM=
endif
LIBRARY=$(LIBRARY_RASPICAM)
HEADERDIR=../../include/yolo/YFramework
LIBDIR=../../lib
BIN=servertest
BIN_SO=libYFramework.so
BIN_ARM=jsontest_arm
OPTION=-std=c++11
OBJECT=YMessage.o YMessageFormatParser.o YHeartbeatManager.o YNetworkManager.o YServoController.o YSonarController.o YCameraController.o
BOOSTLIB=-lboost_system -lboost_thread
ifeq ($(DEFINE), ROBOT)
LINK_LIB_PI=-lwiringPi
else
LINK_LIB_PI=
endif
LINK_OPENCV=-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio
LINK_LIB=$(BOOSTLIB) -lYComm -lpthread $(LINK_LIB_PI) $(LINK_OPENCV) $(LINK_RASPICAM)

#all: $(OBJECT) $(BIN).cc clienttest.cc
#	$(CC) $(OPTION) -o $(BIN) $(INCLUDE) $(BIN).cc $(OBJECT) $(LINK_LIB)
#	$(CC) $(OPTION) -o clienttest $(INCLUDE) clienttest.cc $(OBJECT) $(LINK_LIB)
all: $(OBJECT)
	$(CC) $(OPTION) $(INCLUDE) $(LIBRARY) -shared -o $(BIN_SO) $(OBJECT) $(LINK_LIB)
	sudo cp $(BIN_SO) /usr/local/lib
	mv $(BIN_SO) $(LIBDIR)
YMessage.o: $(HEADERDIR)/YMessage.h YMessage.cpp
	$(CC) $(OPTION) -c -fPIC $(INCLUDE) YMessage.cpp
YMessageFormatParser.o: $(HEADERDIR)/YMessageFormatParser.h YMessageFormatParser.cpp
	$(CC) $(OPTION) -c -fPIC $(INCLUDE) YMessageFormatParser.cpp
YHeartbeatManager.o: $(HEADERDIR)/YHeartbeatManager.h YHeartbeatManager.cpp
	$(CC) $(OPTION) -c -fPIC $(INCLUDE) YHeartbeatManager.cpp
YNetworkManager.o: $(HEADERDIR)/YNetworkManager.h YNetworkManager.cpp
	$(CC) $(OPTION) -c -fPIC $(INCLUDE) YNetworkManager.cpp
YServoController.o: $(HEADERDIR)/YServoController.h YServoController.cpp
	$(CC) $(OPTION) -c -fPIC $(INCLUDE) YServoController.cpp
YSonarController.o: $(HEADERDIR)/YSonarController.h YSonarController.cpp
	$(CC) $(OPTION) -c -fPIC $(INCLUDE) -D$(DEFINE) YSonarController.cpp
YCameraController.o: $(HEADERDIR)/YCameraController.h YCameraController.cpp
	$(CC) $(OPTION) -c -fPIC $(INCLUDE) -D$(DEFINE) YCameraController.cpp

clean:
	rm *.o
