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
set(VERSION 0.1.0)


set(This cxx_plugins)


project(${This}
        VERSION ${VERSION}
        DESCRIPTION "Create/manage external libraries with plugin manager"
        HOMEPAGE_URL "https://github.com/Spaghetti-Software/cxx_plugins"
        LANGUAGES C CXX
        )
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


include(cxx_plugins_options)
include(common_flags)

set(CONAN_SYSTEM_INCLUDE ON)
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
else ()
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
endif ()
conan_basic_setup(TARGETS)

if (BUILD_SHARED_LIBS OR CXX_PLUGINS_SHARED)
    add_library(${This} SHARED)
else ()
    add_library(${This} STATIC)
endif ()

target_sources(${This}
        PRIVATE
        src/polymorphic_allocator.cpp
        src/parser.cpp

        PUBLIC
        include/cxx_plugins/vtable.hpp
        include/cxx_plugins/cxx_plugins.hpp
        include/cxx_plugins/plugin_builder.hpp
        include/cxx_plugins/plugin.hpp
        include/cxx_plugins/variant.hpp
        include/cxx_plugins/function_traits.hpp
        include/cxx_plugins/function_traits.ipp
        include/cxx_plugins/function_cast.hpp
        include/cxx_plugins/functionref.hpp
        include/cxx_plugins/overload_set.hpp
        include/cxx_plugins/type_traits.hpp
        include/cxx_plugins/definitions.hpp
        include/cxx_plugins/polymorphic_cast.hpp
        include/cxx_plugins/polymorphic.hpp
        include/cxx_plugins/parser.hpp
        include/cxx_plugins/polymorphic_traits.hpp
        include/cxx_plugins/polymorphic_ptr.hpp
        include/cxx_plugins/function_proxy.hpp
        include/cxx_plugins/polymorphic_allocator.hpp
        include/cxx_plugins/allocator_helpers.hpp
        include/cxx_plugins/type_index.hpp

        include/cxx_plugins/memory/stack_allocator.hpp
        include/cxx_plugins/memory/mallocator.hpp
        include/cxx_plugins/memory/memory_common.hpp

##      Containers
#       Sequence containers
        include/cxx_plugins/array.hpp
        include/cxx_plugins/vector.hpp
        include/cxx_plugins/deque.hpp
        include/cxx_plugins/forward_list.hpp
        include/cxx_plugins/list.hpp
#       Associative containers
        include/cxx_plugins/set.hpp
        include/cxx_plugins/map.hpp
#       Unordered associative containers
        include/cxx_plugins/unordered_set.hpp
        include/cxx_plugins/unordered_map.hpp
#       Container Adaptors
        include/cxx_plugins/stack.hpp
        include/cxx_plugins/queue.hpp

        include/sequence/size.hpp
        include/sequence/partial_sum.hpp
        include/sequence/exclusive_scan.hpp
        include/sequence/inclusive_scan.hpp
        include/sequence/operators.hpp
        include/sequence/map.hpp
        include/sequence/conversion.hpp

        include/tuple/tuple.hpp
        include/tuple/tuple_cat.hpp
        include/tuple/tuple_declarations.hpp
        include/tuple/tuple_helpers.hpp
        include/tuple/forward.hpp
        include/tuple/tuple_storage.hpp
        include/tuple/tuple_impl.hpp
        include/tuple/tuple_map.hpp

        )

find_package(Threads REQUIRED)

target_include_directories(${This} PRIVATE src)
target_link_libraries(${This} PUBLIC CONAN_PKG::boost ${CMAKE_DL_LIBS} CONAN_PKG::rapidjson CONAN_PKG::fmt)
target_link_libraries(${This} PRIVATE Threads::Threads)

# We determine dev / no dev by checking if tests are enabled
# We need different configurations, so warnings from our library
# are not seen from users compilation
if (CXX_PLUGINS_BUILD_TESTS)
    target_include_directories(${This} PUBLIC include)
else ()
    target_include_directories(${This} INTERFACE SYSTEM include)
    target_include_directories(${This} PRIVATE include)
endif ()

add_subdirectory(example)


if (CXX_PLUGINS_BUILD_TESTS)
    #add_subdirectory(example_plugin)
    add_subdirectory(tests)
endif ()
if (CXX_PLUGINS_BUILD_DOCUMENTATION)
    add_subdirectory(docs)
endif ()
