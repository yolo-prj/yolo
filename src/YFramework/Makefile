CC=g++
ARM_CC=arm-linux-gnueabihf-g++-4.9
INCLUDE=-I/usr/local/include -I../../include
HEADERDIR=../../include/yolo/YFramework
LIBDIR=../../lib
BIN=servertest
BIN_SO=libYFramework.so
BIN_ARM=jsontest_arm
OPTION=-std=c++11
OBJECT=YMessage.o YMessageFormatParser.o YHeartbeatManager.o YNetworkManager.o
BOOSTLIB=-lboost_system -lboost_thread -lpthread
LINK_LIB=$(BOOSTLIB) -lYComm

all: $(OBJECT) $(BIN).cc clienttest.cc
	$(CC) $(OPTION) -o $(BIN) $(INCLUDE) $(BIN).cc $(OBJECT) $(LINK_LIB)
	$(CC) $(OPTION) -o clienttest $(INCLUDE) clienttest.cc $(OBJECT) $(LINK_LIB)
so: $(OBJECT)
	$(CC) $(OPTION) $(INCLUDE) -shared -o $(BIN_SO) $(OBJECT) $(LINK_LIB)
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

clean:
	rm *.o
