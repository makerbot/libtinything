message("configureing tinything")
project(libtinything)

add_CXX_compiler_flags()

set_shared(true)

add_api(TINYTHING)
add_api(JSON)
install_headers(include/)

include_directories(${INCLUDE_OUTPUT_PATH})
#include_directories(../json-cpp/include)
#include_directories(../MBCoreUtils/include)
#include_directories(../libtinything/include)
#include_directories(../libthing-surprise/include)


add_subdirectory(src)

