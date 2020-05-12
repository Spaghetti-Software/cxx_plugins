# [Oh no! More Modern CMake - Deniz Bahadir](https://www.youtube.com/watch?v=y9kSr5enrSk)

> [Modern CMake](https://www.youtube.com/watch?v=y7ndUhdQuU8) (old talk)

## Target requirements

Target requirement types:
**PRIVATE** - declares build-requirement
**INTERFACE** - declares usage-requirement
**PUBLIC** - both

Target commands:
```cmake
target_include_directories( <target> <PRIVATE|INTERFACE|PUBLIC> <include-search-dir>...)
target_compile_definitions( <target> <PRIVATE|INTERFACE|PUBLIC> <macro-definition>...)
target_compile_options( <target> <PRIVATE|INTERFACE|PUBLIC> <compile-option>...)
target_compile_features( <target> <PRIVATE|INTERFACE|PUBLIC> <feature>...)
target_sources( <target> <PRIVATE|INTERFACE|PUBLIC> <source-file>...)
target_precompile_headers( <target> <PRIVATE|INTERFACE|PUBLIC> <header-file>...)
target_link_libraries( <target> <PRIVATE|INTERFACE|PUBLIC> <dependency>...)
target_link_options( <target> <PRIVATE|INTERFACE|PUBLIC> <linker-option>...)
target_link_directories( <target> <PRIVATE|INTERFACE|PUBLIC> <linker-search-dir>...)
```

## Sources

Don't declare sorces inside `add_executable`/`add_library` use `target_sources` instead.
Always add headers to sources.

## `OBJECT` libraries

> `target_link_libraries(lib <PRIVATE|INTERFACE|PUBLIC> obj)` 
> `OBJECT` libraries are not propagated. In other words it is impossible to set usage dependency with `OBJECT` library.
> `target_link_libraries(obj <PRIVATE|INTERFACE|PUBLIC> ob)`
> No build requirements propagated

*Just look int the slides if you want to use `OBJECT` libraries [link](https://youtu.be/y9kSr5enrSk?t=1368)*


## Beginning of each `CMakeLists.txt`

### `cmake_minimum_required`

Always put `cmake_minimum_required` in top of your `CMakeLists.txt`.

Syntax:
```cmake
cmake_minimum_required(VERSION <min-version>...<max-version>)
# or this(will assume that all future versions are supported)
cmake_minimum_required(VERSION <version>)
```

> Preferably use cmake version > 3.15

### Project Settings

include a (generated) file with project settings
```cmake
include(${CMAKE_CURRENT_LIST_DIR}/project-meta-info.in)
```

or use this in the root `CMakeLists.txt`:
```cmake
set(CMAKE_PROJECT_INCLUDE_BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/project-meta-info.in)
```
Then you don't need to type include in every file manually.

> Note: you need to create `project-meta-info.in` in every project dir

### Project


Syntax:
```cmake
project(<project-name>
        VERSION         <MAJOR>.<MINOR>.<PATCH>
        DESCRIPTION     <description>
        HOMEPAGE_URL    <url>
        LANGUAGES       C CXX CUDA
)
```

## External project

> Doesn't apply to us mostly, as we are using git submodules for external libraries
> Though whenever we will need boost we might need this section, as it's a pretty big dependency 
> and we might need to make it differently. 

### Prebuilt binaries

Example with Boost

`external/boost/CMakeLists.txt`
```cmake
set(BOOST_VERSION 1.69.0)

# Settings for finding correct Boost libraries
set(Boost_USE_STATIC_LIBS       FALSE)
set(Boost_USE_MULTITHREADED     TRUE)
set(Boost_USE_STATIC_RUNTIME    FALSE)
# required for module mode only
#set(Boost_ADDITIONAL_VERSIONS   "${BOOST_VERSION}")
set(Boost_COMPILER              "-gcc")

# Search for Boost libraries
find_package(Boost ${BOOST_VERSION} EXACT 
    CONFIG      # Config mode (doesn't always work, needs boost > 1.70)
    REQUIRED
    COMPONENTS  program_options
                graph
)
# Can be this instead
# find_package(Boost ${BOOST_VERSION} EXACT 
#    CONFIG      # Config mode (doesn't always work, needs boost > 1.70)
#    REQUIRED    ALL
# )

# Make found targets globally available
if (Boost_FOUND)
    set_target_properties(  Boost::boost
                            Boost::program_options
                            Boost::graph
        PROPERTIES IMPORT_GLOBAL TRUE
    )
endif()

```

`external/CMakeLists.txt`
```cmake
    add_subdirectory(boost)
```

`CMakeLists.txt`
```cmake
#...
    add_subdirectory(external)
#...
```

+ `Boost_INCLUDE_DIRS` cintaining include-path
+ `Boost_LIBRARIES`, containing file-paths to shared libraries
+ `IMPORTED` targets:
  + `Boost::boost` - headers
  + `Boost::program_options`
  + `Boost::graph`

### Building external dependencies

Demostration with GoogleTest

```cmake

#...

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         release-1.8.0
)

FetchContent_MakeAvailable(googletest)

if (NOT googletest_POPULATED)
    FetchContent_Populate(googletest)

    # Custom policies,variables ...

    add_subdirectory(   ${googletest_SOURCE_DIR} 
                        ${googletest_BINARY_DIR} )

endif()

```
