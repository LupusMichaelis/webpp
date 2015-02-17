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

TESTCXXFLAGS= \
		$(CXXFLAGS) \
		-I=$(HOME)/.local/include/ \

CPPFILES= \
	  url.cpp \
	  json_parser.cpp \
	  json.cpp \
	  mysql_var.cpp \
	  mysql_result.cpp \
	  mysql_connection.cpp \
	  model.cpp \

SRCDIR=src
SRCS=$(addprefix $(SRCDIR)/, $(CPPFILES))
OBJS=$(SRCS:.cpp=.o)

.PHONY: target tests
TARGET=webpp

########################################################################
target: $(TARGET)

$(TARGET): $(OBJS) $(addprefix $(TARGET), .o)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

########################################################################
tests: test_json test_url test_mysql_var
	./$<

# URL tests ############################################################
test_url: tests/test_url.so
	cgreen-runner $^

tests/test_url.so: src/url.o tests/test_url.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -lstdc++ -fPIC

tests/test_url.o: tests/test_url.cpp
	$(CXX) -o $@ -c $^ $(TESTCXXFLAGS)

# MySQL Variable tests #################################################
test_mysql_var: tests/test_mysql_var.so
	cgreen-runner $^

tests/test_mysql_var.so: src/mysql_var.o tests/test_mysql_var.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -lstdc++ -fPIC

tests/test_mysql_var.o: tests/test_mysql_var.cpp
	$(CXX) -o $@ -c $^ $(TESTLDFLAGS)

# JSON parser tests ####################################################
test_json: tests/test_json.so
	cgreen-runner $^

tests/test_json.so: src/json_parser.o src/json.o tests/test_json.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -lstdc++ -fPIC

tests/test_json.o: tests/test_json.cpp
	$(CXX) -o $@ -c $^ $(TESTLDFLAGS)

########################################################################
clean:
	-rm -rf $(OBJS) $(TARGET) $(TARGET:=.o)
