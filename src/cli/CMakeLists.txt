
# Build the cli
add_executable(dot_makerbot_writer writer.cc)

target_link_libraries(dot_makerbot_writer tinything)

export_and_install_targets(TARGETS dot_makerbot_writer NAMESPACE TinyThing)
