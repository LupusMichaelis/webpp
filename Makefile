CXX=g++
MKDIR=mkdir -p

CXXFLAGS= \
		-fPIC \
		-Wall \
		-Werror \
		-std=c++11 \
		`mysql_config --cflags` \
		-g -ggdb \

LDFLAGS= \
		-lboost_signals \
		-lboost_system \
		-lboost_filesystem \
		-lboost_regex \
		`mysql_config --libs` \
		-g -ggdb \


TESTLDFLAGS= \
		-L=$(HOME)/.local/lib64/ \
		-lcgreen \
		-lcgreen++ \
		$(LDFLAGS) \


SRCDIR=src

CPPFILES= \
	  url.cpp \
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

tests: test_json test_url test_mysql_var
	./$<

test_mysql_var: src/mysql_var.o tests/test_mysql_var.o
	$(CXX) -o $@ $^ $(TESTLDFLAGS)

test_url: src/url.o src/test_url.o
	$(CXX) -o $@ $(OBJS) $(TESTLDFLAGS)

# JSON parser tests ####################################################
test_json: tests/test_json.so
	cgreen-runner $^

tests/test_json.so: src/json_parser.o src/json.o tests/test_json.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -lstdc++ -fPIC

tests/test_json.o: tests/test_json.cpp
	$(CXX) -o $@ -c $^ -I=$(HOME)/.local/include/ $(CXXFLAGS)

########################################################################
$(TARGET): $(OBJS) $(addprefix $(TARGET), .o)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	-rm -rf $(OBJS) $(TARGET) $(TARGET:=.o)
