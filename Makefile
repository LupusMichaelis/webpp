
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

CPPFILES= \
	  json.cpp \
	  mysql_result.cpp \
	  mysql_connection.cpp \
	  mysql_var.cpp \
	  model.cpp \

SRCS=$(addprefix $(SRCDIR)/, $(CPPFILES))
OBJS=$(SRCS:.cpp=.o)

.PHONY: target tests
TARGET=webpp

target: $(TARGET)

tests: test_json
	./$@

test_json: src/json.o tests/test_json.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(TARGET): $(OBJS) $(addprefix $(TARGET), .o)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	-rm -rf $(OBJS) $(TARGET) $(TARGET:=.o)
