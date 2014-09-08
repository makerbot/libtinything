cmake_minimum_required(VERSION 2.6)

project(libtinything)

set(COMIPLE_FLAGS
    -std=c++11
    -Wno-unused-local-typedefs
    -pedantic
    -Wall
    -Wextra
    -Wno-variadic-macros
    -Wno-long-long
    -Wswitch-enum
    -O2
    -fPIC)
set(FLAG_STRING "")
foreach(flag ${COMIPLE_FLAGS})
    set(FLAG_STRING "${FLAG_STRING} ${flag}")
endforeach()
set(CMAKE_CXX_FLAGS ${FLAG_STRING})

option(LIBTINYTHING_LIB_BUILD_SHARED
    "Build LIBTINYTHING_lib as a shared library."
    OFF)
if(LIBTINYTHING_LIB_BUILD_SHARED)
  set(LIBTINYTHING_LIB_TYPE SHARED)
  add_definitions( -DJSON_DLL_BUILD )
else()
  set(LIBTINYTHING_LIB_TYPE STATIC)
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build/lib)


add_definitions("\"-DTINYTHING_API=__attribute__ ((visibility (\\\"default\\\")))\"")
add_definitions("\"-DJSON_API=__attribute__ ((visibility (\\\"default\\\")))\"")

include_directories(../json-cpp/include)
include_directories(../MBCoreUtils/include)
include_directories(../libtinything/include)
include_directories(../libthing-surprise/include)


add_subdirectory(src)

