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

add_compiler_flags()

add_api(TINYTHING_API)
use_api(JSON_API)


install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION "${HEADER_INSTALL_DESTINATION}")

add_library(tinything SHARED ${sources} ${headers})

target_link_libraries(tinything
    JsonCpp::jsoncpp)

install_config(
    NAME TinyThing
    TARGET tinything
    API TINYTHING_API)

