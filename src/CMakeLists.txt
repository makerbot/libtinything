#cmake_minimum_required(VERSION 3.19)
#
#project(libtinything)

find_package(JsonCpp REQUIRED)
find_package(ZLIB REQUIRED)
find_package(mb_core_utils REQUIRED)

set(headers
    miniunzip/crypt.h
    miniunzip/ioapi.h
    miniunzip/unzip.h
    miniunzip/zip.h)

set(sources
    tinything_c_interface.cc
    TinyThingConstants.cc
    TinyThingReader.cc
    TinyThingWriter.cc
    miniunzip/ioapi.c
    miniunzip/unzip.c
    miniunzip/zip.c
    cli/writer.cc)

MESSAGE(STATUS "project source dir is ${PROJECT_SOURCE_DIR}")

include_directories(
    "${PROJECT_SOURCE_DIR}/../include"
    "${PROJECT_SOURCE_DIR}/../include/tinything"
    "${PROJECT_SOURCE_DIR}/../src/miniunzip"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/tinything"
    "${PROJECT_SOURCE_DIR}/src/miniunzip"
)

add_library(tinything ${sources} ${headers})

target_link_libraries(tinything JsonCpp::JsonCpp)
target_link_libraries(tinything mb_core_utils::mb_core_utils)

add_subdirectory(cli)
