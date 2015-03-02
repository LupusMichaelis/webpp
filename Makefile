include common.mk

CPPFILES= \
	  http_request.cpp \
	  router.cpp \
	  url.cpp \
	  json_parser.cpp \
	  json.cpp \
	  query_clause.cpp \
	  query_builder.cpp \
	  query.cpp \
	  query_var.cpp \
	  mysql_result.cpp \
	  convert_rows_to_json.cpp \
	  mysql_connection.cpp \
	  model.cpp \

SRCS=$(addprefix $(SRCDIR)/, $(CPPFILES))
OBJS=$(SRCS:.cpp=.o)

.PHONY: target tests all
TARGET=webpp

all: tests target

########################################################################
target: $(TARGET)

$(TARGET): $(OBJS) $(addprefix $(TARGET), .o)
	$(CXX) -o $@ $^ $(LDFLAGS)

########################################################################
tests:
	make -f tests.mk

########################################################################
clean:
	-$(RM) -rf $(OBJS) $(TARGET) $(TARGET:=.o) $(wildcard tests/*.o tests/*.so) core
