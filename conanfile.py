from conans import ConanFile, CMake, tools


class IsRobotsServiceConan(ConanFile):
    name = "is-robots"
    version = "0.0.1"
    license = "MIT"
    url = "https://github.com/labviros/is-robots"
    description = ""
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tests": [True, False],
    }
    default_options = "shared=False", "fPIC=True", "build_tests=False"
    generators = "cmake", "cmake_find_package", "cmake_paths", "virtualrunenv"
    requires = (
        "is-msgs/1.1.8@is/stable",
        "is-wire/1.1.4@is/stable",
        "aria/2.9.4@is/stable",
        "expected/0.3.0@is/stable",
        "zipkin-cpp-opentracing/0.3.1@is/stable",
    )

    exports_sources = "*"

    def build_requirements(self):
        if self.options.build_tests:
            self.build_requires("gtest/1.8.0@bincrafters/stable")

    def configure(self):
        self.options["is-msgs"].shared = True

    def build(self):
        cmake = CMake(self, generator='Ninja')
        cmake.definitions[
            "CMAKE_POSITION_INDEPENDENT_CODE"] = self.options.fPIC
        cmake.definitions["enable_tests"] = self.options.build_tests
        cmake.configure()
        cmake.build()
        # if self.options.build_tests:
        #     cmake.test()

    def package_info(self):
        pass
