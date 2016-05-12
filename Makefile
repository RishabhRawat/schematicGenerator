CC=gcc
CXX=g++
RM=rm -rf
CXXFLAGS= -g -std=c++11 -Wextra -Wall -Iinclude -fPIC
SHARED_LDFLAGS= -shared
#LDLIBS=$(shell root-config --libs)
SRCS=$(wildcard src/*.cpp) 
TEST_SRC=$(wildcard tests/*.cpp)
TEST_EXEC=$(TESTS:.cpp=)
OBJS = $(SRCS:.cpp=.o)

all: schema.so obj-clean

schema.so: $(OBJS)
	$(CXX) -o $@ $^ $(SHARED_LDFLAGS)

test: $(TEST_EXEC)
	@echo $(SRCS)

tests/%.o: tests/%.cpp
	$(CXX) -o $@ $< $(TEST_LDFLAGS)

%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

obj-clean:
	$(RM) $(OBJS)

test-clean:
	$(RM) $(TEST_EXEC)

clean: obj-clean test-clean
	$(RM) schema.so


