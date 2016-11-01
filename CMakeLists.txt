cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

project(schematicGenerator)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wall")
OPTION(EMCC "emcc" OFF)

IF (EMCC)
    set(CMAKE_C_COMPILER "emcc")
    set(CMAKE_CXX_COMPILER "em++")
#    add_definitions( -DDEBUG )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os --memory-init-file 0 -s DISABLE_EXCEPTION_CATCHING=0")

ELSE()
#    set(CMAKE_BUILD_TYPE  "Release")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ")
ENDIF ()

include_directories(include lib)
file(GLOB CORE src/core/*.cpp)
file(GLOB JSON_SUPPORT "src/jsonFrontEnd/*.cpp" "src/javascript/*.cpp")

IF (EMCC)
    file(COPY "tests/test.json" DESTINATION ".")
    add_executable(schema.html ${CORE} ${JSON_SUPPORT} "tests/test1.cpp")
    set_target_properties(schema.html PROPERTIES
            LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file test.json")

    add_executable(schema.js ${CORE} ${JSON_SUPPORT})
    set_target_properties(schema.js PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(schema.js PROPERTIES LINK_FLAGS --bind ${LINK_FLAGS})
    target_compile_definitions(schema.js PRIVATE WEB_COMPILATION )
ELSE ()
    add_library(schematik-core SHARED ${CORE} ${JSON_SUPPORT})

    add_executable(test1 "tests/test1.cpp")
    add_dependencies(test1 schematik-core)
    target_link_libraries(test1 PUBLIC schematik-core)

    add_executable(test3 "tests/test3.cpp")
    add_dependencies(test3 schematik-core)
    target_link_libraries(test3 PUBLIC schematik-core)

    add_executable(test4 "tests/test4.cpp")
    add_dependencies(test4 schematik-core)
    target_link_libraries(test4 PUBLIC schematik-core)
ENDIF ()
#set(CMAKE_VERBOSE_MAKEFILE ON)