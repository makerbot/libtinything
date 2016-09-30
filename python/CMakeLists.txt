# Install the python bindings to libtinything
# Currently only supported for firmware because desktop does not use this

install(
    DIRECTORY tinything DESTINATION "${PY34_MODULE_DIR}"
    FILES_MATCHING PATTERN "*.py")
