#  Copyright (c) 2023 UltiMaker B.V.

# from os import path
from conans import ConanFile, tools
from conan.errors import ConanInvalidConfiguration
from conan.tools.files import AutoPackager, copy, mkdir
from conan.tools.cmake import CMakeToolchain, CMakeDeps, cmake_layout, CMake
from conan.tools.build import check_min_cppstd
from conan.tools.scm import Version


required_conan_version = ">=1.50.0"


class TinyThing(ConanFile):
    name = "tinything"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires= [
        "mb_core_utils/[>=0.1]@makerbot/testing"
    ]
    exports = [
        "CMakeLists.txt",
        "src/*",
        "include/*"
    ]
    generators = "CMakeDeps"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("include/*.h")
        self.copy("include/*.hh")
        lib_patterns = ["*.a", "*.so", "*.dll", "*.lib", "*.lib", "*.dylib"]
        for pattern in lib_patterns:
            self.copy(pattern, dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = tools.collect.libs(self)