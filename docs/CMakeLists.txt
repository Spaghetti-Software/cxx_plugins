
set(DOXYGEN_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/m.css/documentation/doxygen.py)

set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set(m_css_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-mcss.in)
set(m_css ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-mcss)

set(DOXYGEN_STRIP_FROM_PATH ${PROJECT_SOURCE_DIR}/include)
set(DOXYGEN_INPUT "${PROJECT_SOURCE_DIR}/include")
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


configure_file(${doxyfile_in} ${doxyfile} @ONLY)
configure_file(${m_css_in} ${m_css} @ONLY)

set(PROJECT_VER  )
add_custom_target(doc
  COMMAND ${DOXYGEN_EXECUTABLE} ${m_css} --debug
  WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM)
