CC=gcc
CXX=g++
RM=rm -rf
CXXFLAGS= -g -std=c++11 -Wextra -Wall -Iinclude -fPIC
LDFLAGS= -shared
#LDLIBS=$(shell root-config --libs)
SRCS=$(wildcard src/*.cpp) 
OBJS = $(SRCS:.cpp=.o)

all: schema.so obj-clean

schema.so: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

obj-clean:
	$(RM) $(OBJS)

clean: obj-clean
	$(RM) schema.so


