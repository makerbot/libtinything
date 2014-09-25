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

add_library(tinything ${LIB_TYPE} ${sources} ${headers})
set_target_properties( tinything PROPERTIES
    LINKER_LANGUAGE CXX)
    
target_link_libraries(tinything
    jsoncpp)



