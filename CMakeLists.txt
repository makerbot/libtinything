cmake_minimum_required(VERSION 3.19)

project(libtinything)

find_package(JsonCpp REQUIRED)
find_package(ZLIB REQUIRED)
find_package(mb_core_utils REQUIRED)

set(TINYTHING_HEADERS
        src/miniunzip/crypt.h
        src/miniunzip/ioapi.h
        src/miniunzip/unzip.h
        src/miniunzip/zip.h)

set(TINYTHING_SOURCES
        src/tinything_c_interface.cc
        src/TinyThingConstants.cc
        src/TinyThingReader.cc
        src/TinyThingWriter.cc
        src/miniunzip/ioapi.c
        src/miniunzip/unzip.c
        src/miniunzip/zip.c
        src/cli/writer.cc)

include_directories(
        "include"
)

add_library(tinything ${TINYTHING_SOURCES} ${TINYTHING_HEADERS})

target_link_libraries(tinything JsonCpp::JsonCpp)
target_link_libraries(tinything ZLIB::ZLIB)
target_link_libraries(tinything mb_core_utils::mb_core_utils)

set(
        WRITER_SOURCES
        src/cli/writer.cc
)
add_executable(dot_makerbot_writer ${WRITER_SOURCES})
target_link_libraries(dot_makerbot_writer tinything)