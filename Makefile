CC=gcc
CXX=g++
RM=rm -rf
CXXFLAGS= -g -std=c++11 -Wextra -Wall
#LDFLAGS= -g $(shell root-config --ldflags)
#LDLIBS=$(shell root-config --libs)
SRCS=$(wildcard src/*.cpp) 
OBJS = $(SRCS:.cpp=.o)

all: schema obj-clean

schema.out: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

obj-clean:
	$(RM) $(OBJS)

clean: obj-clean
	$(RM) schema


