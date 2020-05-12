# [Effective CMake](https://www.youtube.com/watch?v=bsXLMQ6WgIk)

## Generator expressions
```cmake
target_compile_definitions(foo PRIVATE
    "VERBOSITY=$<IF:$<CONFIG:Debug>,30,10"
)
```
> Evaluated during build system generation

## Get your hands of `CMAKE_CXX_FLAGS`