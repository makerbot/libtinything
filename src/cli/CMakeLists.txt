
# Build the cli
add_executable(dot_makerbot_writer writer.cc)

use_stanadard_flags(dot_makerbot_writer)

target_link_libraries(dot_makerbot_writer tinything)

export_and_install_targets(TARGETS dot_makerbot_writer NAMESPACE TinyThing)
