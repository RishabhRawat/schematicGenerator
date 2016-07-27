cmake_minimum_required(VERSION 2.8)
project(schematicGenerator)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wall")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
OPTION(EMCC "emcc" OFF)

IF (EMCC)
    SET(CMAKE_C_COMPILER "emcc")
    SET(CMAKE_CXX_COMPILER "em++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os --memory-init-file 0 -s DISABLE_EXCEPTION_CATCHING=0")

ELSE()
#    SET(CMAKE_BUILD_TYPE  "Release")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
#    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ")
ENDIF ()

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

INCLUDE_DIRECTORIES(include)
FILE(GLOB CORE src/core/*.cpp)
FILE(GLOB JSON_SUPPORT "src/jsonFrontEnd/*.cpp" "src/javascript/*.cpp")

IF (EMCC)
    file(COPY "tests/test.json" DESTINATION ".")
    ADD_EXECUTABLE(schema.html ${CORE} ${JSON_SUPPORT} "tests/test1.cpp")
    SET_TARGET_PROPERTIES(schema.html PROPERTIES
            LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file test.json")

    ADD_EXECUTABLE(schema.js ${CORE} ${JSON_SUPPORT})
    SET_TARGET_PROPERTIES(schema.js PROPERTIES LINKER_LANGUAGE CXX)
    SET_TARGET_PROPERTIES(schema.js PROPERTIES LINK_FLAGS --bind ${LINK_FLAGS})
    TARGET_COMPILE_DEFINITIONS(schema.js PRIVATE WEB_COMPILATION )
ELSE ()
    ADD_LIBRARY(schema-core SHARED ${CORE})
    ADD_LIBRARY(schema-json SHARED ${CORE} ${JSON_SUPPORT} )

    ADD_EXECUTABLE(test1 "tests/test1.cpp")
    ADD_DEPENDENCIES(test1 schema-json)
    TARGET_LINK_LIBRARIES(test1 PUBLIC schema-json)

    ADD_EXECUTABLE(test3 "tests/test3.cpp")
    ADD_DEPENDENCIES(test3 schema-json)
    TARGET_LINK_LIBRARIES(test3 PUBLIC schema-json)
ENDIF ()
#SET(CMAKE_VERBOSE_MAKEFILE ON)

ADD_CUSTOM_TARGET(formatting
        clang-format-3.8 -i ${CORE} ${JSON_FRONTEND}
        VERBATIM)