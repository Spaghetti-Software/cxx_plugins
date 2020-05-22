###################################################################################################
# Copyright (C) 2020 by Andrey Ponomarev and Timur Kazhimuratov
# This file is part of cxx_plugins project.
# License is available at https://github.com/Spaghetti-Software/cxx_plugins/blob/master/LICENSE
###################################################################################################
#
# \file    CMakeLists.txt
# \author  Andrey Ponomarev
# \date    06 May 2020
# \brief
# Contains setup for test targets for the project

cmake_minimum_required(VERSION 3.15)

set(This CXX_PluginsTests)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
else()
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
endif()
conan_basic_setup(TARGETS)


add_executable(${This})
target_sources(${This}
  PRIVATE
    cxx_plugins_tests.cpp
    function_traits_tests.cpp
    vtable_tests.cpp
    tuple_map_tests.cpp
    example_api.cpp
    example_api.hpp
)


target_link_libraries(${This}
  PUBLIC
    CONAN_PKG::gtest
    cxx_plugins
)
target_include_directories(${This} PUBLIC ${CONAN_INCLUDE_DIRS_BOOST} ${CONAN_INCLUDE_DIRS_GTEST})


add_test(
  NAME ${This}
  COMMAND ${This}
)
