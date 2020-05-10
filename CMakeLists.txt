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
# Main CmakeLists file for cxx plugins project


cmake_minimum_required(VERSION 3.15)

include(cmake/cmake_modules_setup.cmake)

include(get_git_version)

get_git_version(VERSION)

set(This cxx_plugins)


project( ${This}
  VERSION ${VERSION}
)

include(cxx_plugins_options)
include(external_packages_setup)
include(common_flags)

set(Sources
  src/cxx_plugins.cpp
)
set(Headers
  include/cxx_plugins/cxx_plugins.hpp
  include/cxx_plugins/function_traits.hpp
  include/cxx_plugins/function_traits.ipp
)

add_library(${This}
  ${Sources}
  ${Headers}
)


target_include_directories(${This} PRIVATE src)

# We determine dev / no dev by checking if tests are enabled
# We need different configurations, so warnings from our library
# are not seen from users compilation
if (CXX_PLUGINS_BUILD_TESTS)
  target_include_directories(${This} PUBLIC include)
else()
  target_include_directories(${This} INTERFACE SYSTEM include)
  target_include_directories(${This} PRIVATE include)
endif()



if(CXX_PLUGINS_BUILD_TESTS)
  add_subdirectory(tests)
endif()


