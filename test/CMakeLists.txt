find_package(MBCoreUtils REQUIRED)
find_package(JsonCpp REQUIRED)

add_executable(reader_tests
  test_module.cpp
  reader_tests.cpp
  )

target_include_directories(reader_tests
  PRIVATE
  "${MBCoreUtils_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
  )

use_standard_flags(reader_tests)

find_package(
  Boost
  REQUIRED
  CONFIG
  COMPONENTS unit_test_framework
  )

target_link_libraries(reader_tests
  PRIVATE
  tinything
  JsonCpp::jsoncpp
  Boost::unit_test_framework
  )

construct_env_path(
  PREPEND_ENV_VAL_WINDOWS PATH
  PREPEND_ENV_VAL_LINUX LD_LIBRARY_PATH
  PREPEND_ENV_VAL_DARWIN DYLD_LIBRARY_PATH
  VALUES
    "$<TARGET_FILE_DIR:tinything>"
    "$<TARGET_FILE_DIR:JsonCpp::jsoncpp>"
    "$<TARGET_FILE_DIR:Boost::unit_test_framework>"
  PREPEND_ASSIGNMENT
  OUTPUT_VARIABLE environment
  )

file(GLOB test_input_files "${CMAKE_CURRENT_SOURCE_DIR}/*.makerbot")
file(COPY ${test_input_files} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
add_test(
  NAME reader_tests
  COMMAND
  ${CMAKE_COMMAND} -E env "${environment}"
  $<TARGET_FILE:reader_tests> --log_level=message
  )
