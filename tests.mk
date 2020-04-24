
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
	   $(TESTDIR)/test_http_request \
	   $(TESTDIR)/test_http_response \
	   $(TESTDIR)/test_router \
	   $(TESTDIR)/test_json \
	   $(TESTDIR)/test_url \

UNITTESTER=cgreen-runner --colours

.PHONY: all $(TESTS)

all: $(TESTS)

tests/%.so: tests/%.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS) -fPIC

tests/%.o: tests/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

# Query test ###########################################################
$(TESTDIR)/test_query.so: src/query.o src/query_var.o src/query_clause.o src/query_builder.o $(TESTDIR)/test_query.o
$(TESTDIR)/test_query: $(TESTDIR)/test_query.so
	$(UNITTESTER) $^

# Configuration #########################################################
$(TESTDIR)/test_configuration.so: src/configuration.o $(TESTDIR)/test_configuration.o
$(TESTDIR)/test_configuration: $(TESTDIR)/test_configuration.so
	$(UNITTESTER) $^

# HTTP input response ###################################################
$(TESTDIR)/test_http_response.so: src/http_response.o $(TESTDIR)/test_http_response.o
$(TESTDIR)/test_http_response: $(TESTDIR)/test_http_response.so
	$(UNITTESTER) $^

# HTTP input request ###################################################
$(TESTDIR)/test_http_request.so: src/configuration.o src/http_request.o $(TESTDIR)/test_http_request.o
$(TESTDIR)/test_http_request: $(TESTDIR)/test_http_request.so
	$(UNITTESTER) $^

# Router tests #########################################################
$(TESTDIR)/test_router.so: src/router.o $(TESTDIR)/test_router.o
$(TESTDIR)/test_router: $(TESTDIR)/test_router.so
	$(UNITTESTER) $^

# URL tests ############################################################
$(TESTDIR)/test_url.so: src/url.o $(TESTDIR)/test_url.o
$(TESTDIR)/test_url: $(TESTDIR)/test_url.so
	$(UNITTESTER) $^

# MySQL Variable tests #################################################
$(TESTDIR)/test_query_var.so: src/query_var.o $(TESTDIR)/test_query_var.o
$(TESTDIR)/test_query_var: $(TESTDIR)/test_query_var.so
	$(UNITTESTER) $^

# JSON parser tests ####################################################
$(TESTDIR)/test_json.so: src/json_dump.o src/json_parser.o src/json.o $(TESTDIR)/test_json.o
$(TESTDIR)/test_json: $(TESTDIR)/test_json.so
	$(UNITTESTER) $^

# Model test ###########################################################
$(TESTDIR)/test_model.so: src/model.o \
		$(TESTDIR)/test_json.so \
		$(TESTDIR)/test_query.so \
		src/mysql_connection.o src/mysql_result.o \
		$(TESTDIR)/test_model.o
$(TESTDIR)/test_model: $(TESTDIR)/test_model.so
	$(UNITTESTER) $^

########################################################################
clean:
	-$(RM) -rf $(wildcard *.o *.so)
