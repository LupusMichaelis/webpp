CXX=c++
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

TESTDIR=tests
SRCDIR=src

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

