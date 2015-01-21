message("configureing tinything")
cmake_minimum_required(VERSION 2.8.3)
project(libtinything)
set(MBCMakeTools_DIR ${PROJECT_SOURCE_DIR}/../../mw-cmake-tools/)
find_package(
    MBCMakeTools
    REQUIRED)
setup_environment()

add_compiler_flags()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    install_headers($ENV{MB_THIRD_PARTY}/yajl-2.1.0/yajl-${bitness}/include/)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    install_headers(/usr/local/yajl/include/)
endif()

set_shared(true)
add_api(TINYTHING)
add_import_api(JSON)
install_headers(include/)


include_directories(${INCLUDE_OUTPUT_PATH})

include_directories(src/zlib)
include_directories(src/miniunzip)
add_subdirectory(src)
