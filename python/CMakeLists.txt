# Install the python bindings to libtinything
# Currently only supported for firmware because desktop does not use this

if(${MB_CROSS})
    set(python_install_dir "rootfs/${LIB_INSTALL_DIR}/python3.4")
else()
    set(python_install_dir "host/${LIB_INSTALL_DIR}/python3.4")
endif()

install(
    DIRECTORY tinything DESTINATION "${python_install_dir}"
    FILES_MATCHING PATTERN "*.py")
