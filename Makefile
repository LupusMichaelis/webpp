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
		-L$(HOME)/.local/lib64/ \
		-L$(HOME)/.local/lib/ \
		-lcgreen++ \
		$(LDFLAGS) \

TESTCXXFLAGS= \
		$(CXXFLAGS) \
		-I$(HOME)/.local/include/ \

CPPFILES= \
	  url.cpp \
	  json_parser.cpp \
	  json.cpp \
	  mysql_var.cpp \
	  mysql_result.cpp \
	  convert_rows_to_json.cpp \
	  mysql_connection.cpp \
	  model.cpp \

SRCDIR=src
SRCS=$(addprefix $(SRCDIR)/, $(CPPFILES))
OBJS=$(SRCS:.cpp=.o)

.PHONY: target tests all $(TESTS)
TARGET=webpp
TESTS=test_json test_url test_mysql_var

all: tests target

########################################################################
target: $(TARGET)

$(TARGET): $(OBJS) $(addprefix $(TARGET), .o)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

########################################################################
tests: $(TESTS)

# URL tests ############################################################
test_url: tests/test_url.so
	cgreen-runner $^

tests/test_url.so: src/url.o tests/test_url.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -fPIC

tests/test_url.o: tests/test_url.cpp
	$(CXX) -o $@ -c $^ $(TESTCXXFLAGS)

# MySQL Variable tests #################################################
test_mysql_var: tests/test_mysql_var.so
	cgreen-runner $^

tests/test_mysql_var.so: src/mysql_var.o tests/test_mysql_var.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -fPIC

tests/test_mysql_var.o: tests/test_mysql_var.cpp
	$(CXX) -o $@ -c $^ $(TESTCXXFLAGS)

# JSON parser tests ####################################################
test_json: tests/test_json.so
	cgreen-runner $^

tests/test_json.so: src/json_parser.o src/json.o tests/test_json.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -fPIC

tests/test_json.o: tests/test_json.cpp
	$(CXX) -o $@ -c $^ $(TESTCXXFLAGS)

########################################################################
clean:
	-$(RM) -rf $(OBJS) $(TARGET) $(TARGET:=.o) $(wildcard tests/*.o tests/*.so) core
