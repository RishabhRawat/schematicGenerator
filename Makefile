#SET YOSYS_PATH APPROPRIATELY
#IF COMPILING USING EMSCRIPTEN FIRST SOURCE emsdk_env.sh
YOSYS_PATH= ../yosys-core


ifneq '${TARGET}' 'js'
CC=gcc
CXX=g++
else
CC=emcc
CXX=em++
endif

RM=rm -rf
SHARED_CXXFLAGS= -g -std=c++11 -Wextra -Wall -Iinclude -fPIC
SHARED_LDFLAGS= -shared

TEST_CXXFLAGS= -g -std=c++11 -Wextra -Wall -Iinclude
TEST_LDFLAGS=

#LDLIBS=$(shell root-config --libs)
SRCS_CORE=$(wildcard src/core/*.cpp) 
TEST_SRC=$(wildcard tests/*.cpp)
TEST_EXEC=$(basename $(TEST_SRC))
OBJS_CORE = $(SRCS_CORE:.cpp=.o)

ifneq '${TARGET}' 'js'
all: schema.so obj-clean yosys-core-plugin

schema.so: $(OBJS_CORE)
	$(CXX) -o $@ $^ $(SHARED_LDFLAGS)
else
all: schema.js obj-clean

schema.js: $(OBJS_CORE)
	$(CXX) -o $@ $^ $(SHARED_LDFLAGS)
endif

yosys-core-plugin: schema.so
	$(YOSYS_PATH)/yosys-config --exec --cxx --cxxflags -I$(YOSYS_PATH) --ldflags -o genschema.so -shared src/yosys/yosys_pnr_plugin.cpp --ldlibs #-lschema

test: $(TEST_EXEC)

tests/%: tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -o $@ $< $(TEST_LDFLAGS)

%.o: %.cpp
	$(CXX) $(SHARED_CXXFLAGS) -c -o $@ $<

obj-clean:
	$(RM) $(OBJS_CORE)

test-clean:
	$(RM) $(TEST_EXEC)

clean: obj-clean test-clean
	$(RM) schema.so


