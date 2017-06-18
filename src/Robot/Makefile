CC=g++
INCLUDE=-I/usr/local/include -I../../include
HEADERDIR=../../include/yolo/YFramework
LIBDIR=../../lib
BIN=Robot
OPTION=-std=c++11
OBJECT=YMessageSender.o YMessageReceiver.o
BOOSTLIB=-lboost_system -lboost_thread -lpthread
LINK_LIB=$(BOOSTLIB) -lYComm -lYFramework

all: $(OBJECT) $(BIN).cpp
	$(CC) $(OPTION) -o $(BIN) $(INCLUDE) $(BIN).cpp $(OBJECT) $(LINK_LIB)
YMessageSender.o: YMessageSender.h YMessageSender.cpp
	$(CC) $(OPTION) -c $(INCLUDE) YMessageSender.cpp
YMessageReceiver.o: YMessageReceiver.h YMessageReceiver.cpp
	$(CC) $(OPTION) -c $(INCLUDE) YMessageReceiver.cpp

clean:
	rm *.o
	rm $(BIN)
