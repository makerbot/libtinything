set(zlibDir zlib/)
set(zlibSourcs
    ${zlibDir}adler32.c
    ${zlibDir}compress.c
    ${zlibDir}crc32.c
    ${zlibDir}crc32.h
    ${zlibDir}deflate.c
    ${zlibDir}deflate.h
    ${zlibDir}inffast.c
    ${zlibDir}inffast.h
    ${zlibDir}inflate.c
    ${zlibDir}inflate.h
    ${zlibDir}inftrees.c
    ${zlibDir}inftrees.h
    ${zlibDir}trees.h
    ${zlibDir}trees.c
    ${zlibDir}zconf.h
    ${zlibDir}zconf.in.h
    ${zlibDir}zlib.h
    ${zlibDir}zuitl.c
    ${zlibDir}zutil.h)

set(miniunzipDir miniunzip/)
set(miniunzipsSources
    ${miniunzipDir}crypt.h
    ${miniunzipDir}ioapi.c
    ${miniunzipDir}ioapi.h
    ${miniunzipDir}unzip.c
    ${miniunzipDir}unzip.h
    ${miniunzipDir}zip.c
    ${miniunzipDir}zip.h)

set(cliDir cli/)
set(cliSources
    ${cliDir}writer.cc)

add_library( libtinything_lib ${LIBTINYTHING_LIB_TYPE}
    tinything_c_interface.cc
    TinyThingConstants.cc
    TinyThingReader.cc
    TinyThingWriter.cc
    ${zlibSources}
    ${cliSources}
    ${miniunzipsSources} )

set_target_properties( libtinything_lib PROPERTIES
    OUTPUT_NAME libtinything
    LINKER_LANGUAGE CXX
    PREFIX "" )
# Install instructions for this target
install( TARGETS libtinything_lib
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/build/bin
    LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/build/lib
    ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/build/lib )


