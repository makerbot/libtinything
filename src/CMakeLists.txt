
if(DEFINED BIRDWING AND DEFINED MB_CROSS)
    find_package(JsonCpp REQUIRED
        NO_DEFAULT_PATH
        PATHS
            ${CMAKE_INSTALL_PREFIX}/cross_cmake)
else()
    find_package(JsonCpp REQUIRED)
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # ensure the slash direction
    string(
        REPLACE "\\" "/"
        yajl_include_dir
        "$ENV{MB_THIRD_PARTY}/yajl-2.1.0/yajl-${bitness_32or64}/include/")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(yajl_include_dir /usr/local/yajl/include/)
endif()

set(headers
    zlib/crc32.h
    zlib/deflate.h
    zlib/inffast.h
    zlib/inflate.h
    zlib/inftrees.h
    zlib/zconf.h
    zlib/zconf.in.h
    zlib/zlib.h
    zlib/trees.h
    zlib/zutil.h

    miniunzip/crypt.h
    miniunzip/ioapi.h
    miniunzip/unzip.h
    miniunzip/zip.h)
set(sources
    tinything_c_interface.cc
    TinyThingConstants.cc
    TinyThingReader.cc
    TinyThingWriter.cc

    zlib/adler32.c
    zlib/compress.c
    zlib/crc32.c
    zlib/deflate.c
    zlib/inffast.c
    zlib/inflate.c
    zlib/inftrees.c
    zlib/trees.c
    zlib/zutil.c

    miniunzip/ioapi.c
    miniunzip/unzip.c
    miniunzip/zip.c
    cli/writer.cc)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/src/zlib"
    "${PROJECT_SOURCE_DIR}/src/miniunzip"
    ${yajl_include_dir})

add_library(tinything ${sources} ${headers})
use_standard_flags(tinything)
export_api(tinything TINYTHING_API)

# Prevent zlib from doing its usual API stuff
target_compile_definitions(tinything PUBLIC ZEXPORT= ZEXTERN=)

target_link_libraries(tinything JsonCpp::jsoncpp)

export_and_install_targets(TARGETS tinything NAMESPACE TinyThing)

add_subdirectory(cli)

generate_and_install_config(NAME TinyThing)

get_header_prefix_root(install_root)

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION "${install_root}${HEADER_INSTALL_DIR}")

install(
    DIRECTORY ${yajl_include_dir}
    DESTINATION "${install_root}${HEADER_INSTALL_DIR}")
