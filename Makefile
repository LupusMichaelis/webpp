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
	  http_request.cpp \
	  router.cpp \
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

TARGET=webpp
TESTS= \
	   test_model \
	   test_router \
	   test_json \
	   test_url \
	   test_mysql_var \

.PHONY: target tests all $(TESTS)
all: tests target

########################################################################
target: $(TARGET)

$(TARGET): $(OBJS) $(addprefix $(TARGET), .o)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

########################################################################
tests: $(TESTS)

# HTTP input request ###################################################
test_http_request: tests/test_http_request.so
	cgreen-runner $^

tests/test_http_request.so: src/http_request.o tests/test_http_request.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -fPIC

tests/test_http_request.o: tests/test_http_request.cpp
	$(CXX) -o $@ -c $^ $(TESTCXXFLAGS)

# Router tests #########################################################
test_router: tests/test_router.so
	cgreen-runner $^

tests/test_router.so: src/router.o tests/test_router.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -fPIC

tests/test_router.o: tests/test_router.cpp
	$(CXX) -o $@ -c $^ $(TESTCXXFLAGS)

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

# Model test ###########################################################
test_model: tests/test_model.so
	cgreen-runner $^

tests/test_model.so: src/model.o src/json.o src/json_parser.o src/mysql_connection.o src/mysql_result.o src/mysql_var.o tests/test_model.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(TESTLDFLAGS) -fPIC

tests/test_model.o: tests/test_model.cpp
	$(CXX) -o $@ -c $^ $(TESTCXXFLAGS)

########################################################################
clean:
	-$(RM) -rf $(OBJS) $(TARGET) $(TARGET:=.o) $(wildcard tests/*.o tests/*.so) core
