
# Build the cli
add_executable(tinything_writer writer.cc)

target_link_libraries(tinything_writer tinything)

export_and_install_targets(TARGETS tinything_writer NAMESPACE TinyThing)
