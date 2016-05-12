CC=gcc
CXX=g++
RM=rm -rf
SHARED_CXXFLAGS= -g -std=c++11 -Wextra -Wall -Iinclude -fPIC
SHARED_LDFLAGS= -shared

TEST_CXXFLAGS= -g -std=c++11 -Wextra -Wall -Iinclude
TEST_LDFLAGS=

#LDLIBS=$(shell root-config --libs)
SRCS=$(wildcard src/*.cpp) 
TEST_SRC=$(wildcard tests/*.cpp)
TEST_EXEC=$(basename $(TEST_SRC))
OBJS = $(SRCS:.cpp=.o)

all: schema.so obj-clean

schema.so: $(OBJS)
	$(CXX) -o $@ $^ $(SHARED_LDFLAGS)

test: $(TEST_EXEC)

tests/%: tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -o $@ $< $(TEST_LDFLAGS)

%.o: %.cpp
	$(CXX) $(SHARED_CXXFLAGS) -c -o $@ $<

obj-clean:
	$(RM) $(OBJS)

test-clean:
	$(RM) $(TEST_EXEC)

clean: obj-clean test-clean
	$(RM) schema.so


