from conans import ConanFile, CMake, tools


class CxxPlugins(ConanFile):
    name = "cxx_plugins"
    version = "0.1.6"
    license = "MIT"
    url = "https://github.com/Spaghetti-Software/cxx_plugins"
    description = "CXX Plugins is a library that's main aim is to simplify dealing with API and external dll loading."
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    options = {"shared": [True, False], "enable_tests": [True, False], "enable_documentation": [True, False]}
    default_options = {"shared": False, "enable_tests": False, "enable_documentation": False}
    requires = ["boost/1.73.0", "rapidjson/1.1.0", "fmt/7.0.3"]
    exports_sources = "*", "!.github", "!.vscode", "!build"

    def configure(self):
        if self.settings.compiler == "clang" or self.settings.compiler == "gcc":
            if self.settings.compiler.libcxx == "libstdc++":
                raise Exception("This package is only compatible with libstdc++11")

    def package_id(self):
        del self.info.options.enable_tests
        del self.info.options.enable_documentation

    # def source(self):
    #     # self.run("git clone -b dev/cxx_plugins/plugins https://github.com/Spaghetti-Software/cxx_plugins_mirror.git .")
    #     pass

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

    def package(self):
        self.copy("*", dst="include", src="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["cxx_plugins"]
