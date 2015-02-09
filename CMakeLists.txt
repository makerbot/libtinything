message("configureing tinything")
cmake_minimum_required(VERSION 2.8.3)
project(libtinything CXX)

find_package(
    MBCMakeTools
    REQUIRED)

find_package(
    JsonCpp
    REQUIRED)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directory($ENV{MB_THIRD_PARTY}/yajl-2.1.0/yajl-${bitness}/include/)
    install(
        DIRECTORY "$ENV{MB_THIRD_PARTY}/yajl-2.1.0/yajl-${bitness}/include/"
        DESTINATION "${HEADER_INSTALL_DESTINATION}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    include_directory(/usr/local/yajl/include/)
    install(
        DIRECTORY "/usr/local/yajl/include/"
        DESTINATION "${HEADER_INSTALL_DESTINATION}")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(src/zlib)
include_directories(src/miniunzip)

add_subdirectory(src)
