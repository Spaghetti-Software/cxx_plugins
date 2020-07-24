cmake_minimum_required(VERSION 3.15)

project(ExamplePlugin)

include(GenerateExportHeader)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)




add_library(example_api INTERFACE)
target_include_directories(example_api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/example_plugin.json"
  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/example_plugin.json"
  COPYONLY
)
add_library(example_plugin SHARED
        source.cpp
        api.hpp
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/example_plugin.json"
        )

target_include_directories(example_plugin
PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)



target_link_libraries(example_plugin PRIVATE cxx_plugins)

generate_export_header(example_plugin)