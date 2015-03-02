
include common.mk

LDFLAGS += \
		-L$(HOME)/.local/lib64/ \
		-L$(HOME)/.local/lib/ \
		-lcgreen++ \

CXXFLAGS += \
		-I$(HOME)/.local/include/ \

TESTS= \
	   $(TESTDIR)/test_query_var \
	   $(TESTDIR)/test_query \
	   $(TESTDIR)/test_model \
	   $(TESTDIR)/test_router \
	   $(TESTDIR)/test_json \
	   $(TESTDIR)/test_url \

UNITTESTER=cgreen-runner --colours

.PHONY: all

all: $(TESTS)

# Query test ###########################################################
tests/test_query: tests/test_query.so
	$(UNITTESTER) $^

tests/test_query.so: src/query.o src/query_var.o src/query_clause.o src/query_builder.o tests/test_query.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS) -fPIC

# HTTP input request ###################################################
tests/test_http_request: tests/test_http_request.so
	$(UNITTESTER) $^

tests/test_http_request.so: src/http_request.o tests/test_http_request.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS) -fPIC

# Router tests #########################################################
tests/test_router: tests/test_router.so
	$(UNITTESTER) $^

tests/test_router.so: src/router.o tests/test_router.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS) -fPIC

# URL tests ############################################################
tests/test_url: tests/test_url.so
	$(UNITTESTER) $^

tests/test_url.so: src/url.o tests/test_url.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS) -fPIC

# MySQL Variable tests #################################################
tests/test_query_var: tests/test_query_var.so
	$(UNITTESTER) $^

tests/test_query_var.so: src/query_var.o tests/test_query_var.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS) -fPIC

# JSON parser tests ####################################################
tests/test_json: tests/test_json.so
	$(UNITTESTER) $^

tests/test_json.so: src/json_parser.o src/json.o tests/test_json.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS) -fPIC

# Model test ###########################################################
tests/test_model: tests/test_model.so
	$(UNITTESTER) $^

tests/test_model.so: src/model.o src/json.o src/json_parser.o src/mysql_connection.o src/mysql_result.o src/query_var.o tests/test_model.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS) -fPIC

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

########################################################################
clean:
	-$(RM) -rf $(wildcard *.o *.so)
