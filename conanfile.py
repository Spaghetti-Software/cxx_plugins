from conans import ConanFile, CMake, tools


class CxxPlugins(ConanFile):
    name = "cxx-plugins"
    version = "0.0.1"
    license = "MIT"
    url = "https://github.com/Spaghetti-Software/cxx_plugins"
    description = "CXX Plugins is a library that's main aim is to simplify dealing with API and external dll loading."
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    options = {"shared": [True, False], "enable_tests": [True, False], "enable_documentation": [True, False]}
    default_options = {"shared": False, "enable_tests": False, "enable_documentation": False}

    def package_id(self):
        del self.info.options.enable_tests
        del self.info.options.enable_documentation

    def build(self):
        cmake = CMake(self)
        cmake.definitions['CXX_PLUGINS_BUILD_TESTS'] = 'ON' if self.options.enable_tests else 'OFF'
        cmake.definitions['CXX_PLUGINS_SHARED'] = 'ON' if self.options.shared else 'OFF'
        cmake.definitions['CXX_PLUGINS_BUILD_DOCUMENTATION'] = 'ON' if self.options.enable_documentation else 'OFF'

        cmake.configure()
        cmake.build()
        if self.options.enable_tests:
            cmake.test()

    def requirements(self):
        if self.options.enable_tests:
            self.requires('gtest/1.10.0')
