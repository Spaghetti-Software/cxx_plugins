find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
  message(FATAL_ERROR "Doxygen is needed to build th edocumentation")
endif()

set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})


configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_target(doc
  COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
  WORKING_DIRECTORY ${OUT_DIR}
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM)
install(DIRECTORY ${OUT_DIR}/html DESTINATION docs)
set(A PROJECT_)