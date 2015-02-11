
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
	  json_parser.cpp \
	  json.cpp \
	  mysql_var.cpp \
	  mysql_result.cpp \
	  mysql_connection.cpp \
	  model.cpp \

SRCS=$(addprefix $(SRCDIR)/, $(CPPFILES))
OBJS=$(SRCS:.cpp=.o)

.PHONY: target tests
TARGET=webpp

target: $(TARGET)

tests: test_json test_mysql_var
	./$<

test_mysql_var: src/mysql_var.o tests/test_mysql_var.o
	$(CXX) -o $@ $^ $(LDFLAGS)

test_json: src/json_parser.o src/json.o tests/test_json.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(TARGET): $(OBJS) $(addprefix $(TARGET), .o)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	-rm -rf $(OBJS) $(TARGET) $(TARGET:=.o)
