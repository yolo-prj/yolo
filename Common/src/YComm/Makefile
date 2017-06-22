GCC=g++
TARGET=libYComm.so
OPTION=-std=c++11
BOOSTLIBDIR=/usr/local/lib
LIBS=-L/usr/local/lib
#PREPROCESSOR=-D YCOMM_EXPORTS -D _UNICODE -D _WIN32_WINNT=0x501
PREPROCESSOR=-D YCOMM_EXPORTS -D _UNICODE
MAINDIR=../../
LIBSDIR=$(MAINDIR)lib/
HEADERDIR=$(MAINDIR)include/yolo/YComm/
DEPENDENCY=$(HEADERDIR)YCommCommon.h $(HEADERDIR)YCommMain.h
INCLUDES=-I $(MAINDIR)include/yolo -I $(MAINDIR)include -I /usr/local/include/boost -I /usr/local/include
OBJS=YComm.o YTcpClient.o YTcpServer.o YTcpSession.o YUdp.o YUdpReceiveDatagramHandler.o

##if defined(__APPLE__) && defined(__MACH__)

OS := $(shell uname)
ifeq ($(OS),Darwin)  # Mac OS X
#LIBS+=-lboost_system -lboost_thread-mt -lboost_exception -lboost_regex -lboost_timer
BOOSTLIBS=$(BOOSTLIBDIR)/libboost_system.dylib $(BOOSTLIBDIR)/libboost_thread-mt.dylib $(BOOSTLIBDIR)/libboost_exception.a $(BOOSTLIBDIR)/libboost_regex.dylib $(BOOSTLIBDIR)/libboost_timer.dylib
endif

ifeq ($(OS),Linux)
BOOSTLIBS=$(BOOSTLIBDIR)/libboost_system.so $(BOOSTLIBDIR)/libboost_thread.so $(BOOSTLIBDIR)/libboost_exception.a $(BOOSTLIBDIR)/libboost_regex.so $(BOOSTLIBDIR)/libboost_timer.so
endif


all: $(TARGET)

$(TARGET) : $(OBJS)
	$(GCC) $(INCLUDES) -shared -o $(TARGET) $(OBJS) $(BOOSTLIBS)
	sudo cp $(TARGET) /usr/local/lib

#$(OBJS): YComm.o YTcpClient.o YTcpServer.o YTcpSession.o YUdp.o YUdpReceiveDatagramHandler.o

YComm.o: $(HEADERDIR)YComm.h YComm.cpp $(DEPENDENCY)
	$(GCC) $(OPTION) $(INCLUDES) $(LIBS) $(PREPROCESSOR) -c -fPIC YComm.cpp

YTcpSession.o: $(HEADERDIR)YTcpSession.h YTcpSession.cpp $(DEPENDENCY)
	$(GCC) $(OPTION) $(INCLUDES) $(LIBS) $(PREPROCESSOR) -c -fPIC YTcpSession.cpp

YTcpClient.o: $(HEADERDIR)YTcpClient.h YTcpClient.cpp $(DEPENDENCY) YTcpSession.o
	$(GCC) $(OPTION) $(INCLUDES) $(LIBS) $(PREPROCESSOR) -c -fPIC YTcpClient.cpp

YTcpServer.o: $(HEADERDIR)YTcpServer.h YTcpServer.cpp $(DEPENDENCY) YTcpSession.o
	$(GCC) $(OPTION) $(INCLUDES) $(LIBS) $(PREPROCESSOR) -c -fPIC YTcpServer.cpp

YUdp.o: $(HEADERDIR)YUdp.h YUdp.cpp $(DEPENDENCY) YUdpReceiveDatagramHandler.o
	$(GCC) $(OPTION) $(INCLUDES) $(LIBS) $(PREPROCESSOR) -c -fPIC YUdp.cpp

YUdpReceiveDatagramHandler.o: $(HEADERDIR)YUdpReceiveDatagramHandler.h YUdpReceiveDatagramHandler.cpp $(DEPENDENCY)
	$(GCC) $(OPTION) $(INCLUDES) $(LIBS) $(PREPROCESSOR) -c -fPIC YUdpReceiveDatagramHandler.cpp



clean:
	rm -rf *.o
