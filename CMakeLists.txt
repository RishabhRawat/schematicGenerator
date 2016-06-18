cmake_minimum_required(VERSION 3.5)
project(schematicGenerator)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wall")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
OPTION(EMCC "emcc" OFF)

IF (EMCC)
    SET(CMAKE_C_COMPILER "emcc")
    SET(CMAKE_CXX_COMPILER "em++")
ELSE()
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
ENDIF ()

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

INCLUDE_DIRECTORIES(include)
FILE(GLOB CORE src/core/*.cpp)
FILE(GLOB JSON_FRONTEND "src/jsonFrontEnd/*.cpp")
FILE(GLOB JAVASCRIPT "src/javascript/*.cpp" "src/jsonFrontEnd/*.cpp")

IF (EMCC)
    file(COPY "tests/test.json" DESTINATION ".")
    ADD_EXECUTABLE(schema.html ${CORE} ${JSON_FRONTEND} "tests/test1.cpp")
    SET_TARGET_PROPERTIES(schema.html PROPERTIES
            LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file test.json")
    ADD_EXECUTABLE(schema.js ${CORE} ${JAVASCRIPT})
    SET_TARGET_PROPERTIES(schema.js PROPERTIES LINKER_LANGUAGE CXX)
    SET_TARGET_PROPERTIES(schema.js PROPERTIES LINK_FLAGS --bind ${LINK_FLAGS})
    TARGET_COMPILE_DEFINITIONS(schema.js PRIVATE WEB_COMPILATION )
ELSE ()
    ADD_LIBRARY(schema-core SHARED ${CORE})
    ADD_LIBRARY(schema-json SHARED ${CORE} ${JSON_FRONTEND})

    ADD_EXECUTABLE(test1 "tests/test1.cpp")
    ADD_DEPENDENCIES(test1 schema-json)
    TARGET_LINK_LIBRARIES(test1 PUBLIC schema-json)
ENDIF ()


ADD_CUSTOM_TARGET(formatting
        clang-format-3.8 -i ${CORE} ${JSON_FRONTEND}
        VERBATIM)