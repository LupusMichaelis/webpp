
CXX=g++
MKDIR=mkdir -p

CXXFLAGS=-Wall \
		-Werror \
		-std=c++11 \
		`mysql_config --cflags` \
		-g -ggdb

LDFLAGS= \
		-lboost_signals \
		-lboost_system \
		-lboost_filesystem \
		-lboost_regex \
		`mysql_config --libs` \
		-g -ggdb

SRCDIR=src

SRCS= \
	  json.cpp \
	  mysql_result.cpp \
	  mysql_connection.cpp \
	  mysql_var.cpp \
	  model.cpp \

CPPFILES=$(addprefix $(SRCDIR)/, $(SRCS))
OBJS=$(SRCS:.cpp=.o)

.PHONY: tests
TARGET=webpp

target: $(TARGET)

webpp: $(OBJS)
	$(CXX) -o $@ $(addprefix $@, .o) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

clean:
	-rm -rf $(OBJS) $(TARGET) $(TARGET:=.o)
