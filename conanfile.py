#  Copyright (c) 2023 UltiMaker B.V.

# from os import path
#
from conans import ConanFile, CMake
# from conan.errors import ConanInvalidConfiguration
# from conan.tools.files import AutoPackager, copy, mkdir
# from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake, cmake_layout
# from conan.tools.build import check_min_cppstd
# from conan.tools.scm import Version


required_conan_version = ">=1.56.0"


class TinyThing(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires= [
        # "jsoncpp/[>=1.9.5]",
        "mb_core_utils/[>=0.1]@makerbot/testing"
    ]
    generators = "CMakeDeps"

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin") # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
