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

set(Sources
  cxx_plugins_tests.cpp
)

add_executable(${This} ${Sources})
target_link_libraries(${This} 
  PUBLIC
    gtest
    cxx_plugins
)
target_compile_features(${This} PRIVATE cxx_std_20)

add_test(
  NAME ${This}
  COMMAND ${This}
)