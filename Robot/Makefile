CC=g++
DEFINE=ROBOT
#DEFINE=RUI
ifeq ($(DEFINE), ROBOT)
	INCLUDE_RASPICAM=-I/home/pi/git/robidouille/raspicam_cv
	LIB_RASPICAM=-L/home/pi/git/robidouille/raspicam_cv
else
	INCLUDE_RASPICAM=
	LIB_RASPICAM=
endif

INCLUDE=-I/usr/local/include -I../Common/include $(INCLUDE_RASPICAM)

LIB_PATH=$(LIB_RASPICAM) -L/usr/local/lib

BIN=Robot

OPTION=-std=c++11 -o3


ifeq ($(DEFINE), ROBOT)
	LINK_RASPICAM=-lraspicamcv
else
	LINK_RASPICAM=
endif


OS := $(shell uname)
ifeq ($(OS),Darwin)  # Mac OS X
BOOSTLIB=-lboost_system -lboost_thread-mt -lrt -lpthread
LINK_OPENCV=-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_videostab
endif

ifeq ($(OS),Linux)
BOOSTLIB=-lboost_system -lboost_thread -lrt -lpthread
LINK_OPENCV=-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio
endif



LINK_LIB=$(BOOSTLIB) -lYComm -lYFramework $(LINK_OPENCV) $(LINK_RASPICAM)



SRCS=YMessageSender.cpp YMessageReceiver.cpp RobotManager.cpp LineDetect.cpp Vision.cpp main.cpp SignHandler.cpp


OBJS = $(SRCS:.cpp=.o)

all: $(OBJS)
	$(CC) $(OPTION) -o $(BIN) $(INCLUDE) $(LIB_PATH) $(OBJS) $(LINK_LIB)
 
%.o:%.cpp
	$(CC) $(OPTION) -c $(INCLUDE) -c $< -o $@

clean:
	-@-rm *.o
	-@-rm $(BIN)
