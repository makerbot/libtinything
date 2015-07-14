
cmake_minimum_required(VERSION 2.8.3)

project(libtinything C CXX)

find_package(MBCMakeTools REQUIRED)

add_subdirectory(src)

install_docs(
    FILES "${PROJECT_SOURCE_DIR}/README.md"
    PREFIX tinything)
