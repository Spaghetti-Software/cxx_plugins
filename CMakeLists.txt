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

#include(get_git_version)
#get_git_version(VERSION)
set(VERSION 0.0.1)


set(This cxx_plugins)


project( ${This}
  VERSION ${VERSION}
  DESCRIPTION     "Create/manage external libraries with plugin manager"
  HOMEPAGE_URL    "https://github.com/Spaghetti-Software/cxx_plugins"
  LANGUAGES       C CXX
)


include(cxx_plugins_options)
include(common_flags)




if (BUILD_SHARED_LIBS OR CXX_PLUGINS_SHARED)
  add_library(${This} SHARED)
else()
  add_library(${This} STATIC )
endif()

set(CONAN_SYSTEM_INCLUDE ON)
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
else ()
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
endif ()
conan_basic_setup(TARGETS)

target_sources(${This}
        PRIVATE
        src/cxx_plugins.cpp
        PUBLIC
        include/cxx_plugins/vtable.hpp
        include/cxx_plugins/cxx_plugins.hpp
        include/cxx_plugins/function_traits.hpp
        include/cxx_plugins/function_traits.ipp
        include/cxx_plugins/type_traits.hpp
        include/cxx_plugins/tuple.hpp
        include/cxx_plugins/definitions.hpp
        include/cxx_plugins/polymorphic.hpp
        include/cxx_plugins/tuple_map.hpp
        include/cxx_plugins/tuple_impl.ipp

        include/sequence/size.hpp
        include/sequence/partial_sum.hpp
        include/sequence/exclusive_scan.hpp
        include/sequence/inclusive_scan.hpp
        include/sequence/operators.hpp
        include/sequence/map.hpp

        include/cxx_plugins/tuple_impl/tuple_cat.hpp
        include/cxx_plugins/tuple_impl/tuple_declarations.hpp
        )

target_include_directories(${This} PRIVATE src)
target_link_libraries(${This} INTERFACE CONAN_PKG::boost)

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
  #add_subdirectory(example_plugin)
  add_subdirectory(tests)
endif()
if(CXX_PLUGINS_BUILD_DOCUMENTATION)
  add_subdirectory(docs)
endif()


