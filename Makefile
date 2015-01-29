
CXXFLAGS=-Wall \
		-Werror \
		-std=c++11 \
		`mysql_config --cflags` \
		-g -ggdb

LDFLAGS= \
		-lboost_signals \
		-lboost_system \
		-lboost_filesystem \
		`mysql_config --libs` \
		-g -ggdb

SRCS= \
	  story.cpp \
	  model.cpp \
	  webpp.cpp \


OBJS=$(SRCS:.cpp=.o)

TARGET=webpp

target: $(TARGET)

webpp: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

clean:
	-rm -rf $(OBJS) $(TARGET) $(TARGET:=.o)
