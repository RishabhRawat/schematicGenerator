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
TEST_LDFLAGS=  -Wl,-R -Wl,$(shell pwd)
TEST_LDLIBS = libschema.so

#LDLIBS=$(shell root-config --libs)
SRCS_CORE=$(wildcard src/core/*.cpp) $(wildcard src/jsonFrontEnd/*.cpp) 
TEST_SRC=$(wildcard tests/*.cpp)
TEST_EXEC=$(basename $(TEST_SRC))
OBJS_CORE = $(SRCS_CORE:.cpp=.o)

ifneq '${TARGET}' 'js'
all: libschema.so obj-clean test yosys-core-plugin

libschema.so: $(OBJS_CORE)
	$(CXX) $(SHARED_LDFLAGS) -o $@ $^ 
else
all: schema.js obj-clean

schema.js: $(OBJS_CORE)
	$(CXX) -o $@ $^ $(SHARED_LDFLAGS)
endif

yosys-core-plugin: libschema.so
	$(YOSYS_PATH)/yosys-config --exec --cxx --cxxflags -I$(YOSYS_PATH) --ldflags -o yosys-schema-plugin.so -shared src/yosys/yosys_pnr_plugin.cpp --ldlibs #-lschema

test: $(TEST_EXEC)

tests/%: tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) $(TEST_LDFLAGS) -o $@ $< $(TEST_LDLIBS)

%.o: %.cpp
	$(CXX) $(SHARED_CXXFLAGS) -c -o $@ $<

obj-clean:
	$(RM) $(OBJS_CORE)

test-clean:
	$(RM) $(TEST_EXEC)

yosys-clean:
	$(RM) yosys-schema-plugin.so yosys-schema-plugin.d

clean: obj-clean test-clean yosys-clean
	$(RM) libschema.so


