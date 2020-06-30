option(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(COMMON_NIX_FLAGS
  "-Weffc++ -Wall -Wextra -pedantic"
)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

  set(SANITIZE_FLAGS "-fsanitize=undefined -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_NIX_FLAGS} ${SANITIZE_FLAGS}")

  set(LINKER_FLAGS ${SANITIZE_FLAGS})
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZE_FLAGS}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(COMMON_COMPILER_FLAGS 
    ${COMMON_NIX_FLAGS}
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  
endif()