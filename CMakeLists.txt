
cmake_minimum_required(VERSION 3.1)

project(libtinything C CXX)

find_package(MBCMakeTools REQUIRED NO_POLICY_SCOPE)

add_subdirectory(src)

if(${BIRDWING})
    add_subdirectory(python)
endif()

install_docs(
    FILES "${PROJECT_SOURCE_DIR}/README.md"
    PREFIX tinything)
