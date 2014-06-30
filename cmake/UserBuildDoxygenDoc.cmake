

MACRO(USER_BUILD_DOXYGEN_DOC NAME INPUT DOC_RELATIVE_INSTALL_PATH PREDEFINED)

  #--------------------------------------------------------------------------
  SET(USE_DOXYGEN ON CACHE BOOL "" FORCE)
  
  # Name
  SET(DOXYGEN_PROJECT_NAME "${NAME}")

  # Inputs
  STRING(REGEX REPLACE ";" " " DOXYGEN_INPUT "${INPUT}")

  # Output dirs
  SET(DOXYGEN_HTML_OUTPUT ".")
  SET(DOXYGEN_OUTPUT "${${PROJECT_NAME}_DOXYGEN_BUILD_PATH}") 
  STRING(REGEX REPLACE " " "~" DOXYGEN_OUTPUT "${DOXYGEN_OUTPUT}")
  
 
  IF(NOT IS_DIRECTORY ${DOXYGEN_OUTPUT}/${DOXYGEN_HTML_OUTPUT})
    FILE(MAKE_DIRECTORY ${DOXYGEN_OUTPUT}/${DOXYGEN_HTML_OUTPUT})
  ENDIF(NOT IS_DIRECTORY ${DOXYGEN_OUTPUT}/${DOXYGEN_HTML_OUTPUT})

  # Doc exclude
  SET(DOXYGEN_EXCLUDE "")
  STRING(REGEX REPLACE ";" " " DOXYGEN_EXCLUDE "${DOXYGEN_EXCLUDE}")

  # Log file name 
  SET(DOXYGEN_LOGFILE "${CMAKE_CURRENT_BINARY_DIR}/doxygen.log")

  # Predefined symbols
  STRING(REGEX REPLACE ";" " " DOXYGEN_DOC_PREDEFINED "${PREDEFINED}")
  
  #---------------------------------------------------------------------------
  # DOT verification
  IF(DOT)
    GET_FILENAME_COMPONENT(DOXYGEN_DOT_PATH ${DOT} PATH)
    SET(DOXYGEN_HAVE_DOT "YES")
  ELSE(DOT)
    SET(DOXYGEN_DOT_PATH "")
    SET(DOXYGEN_HAVE_DOT "NO")
  ENDIF(DOT)
  
  #---------------------------------------------------------------------------
  # Create file and project
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.txt.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt
    @ONLY IMMEDIATE
    )
 
  
  ADD_CUSTOM_COMMAND(
    OUTPUT ${DOXYGEN_OUTPUT}/${DOXYGEN_HTML_OUTPUT}/index.html
    COMMAND 
    ${DOXYGEN}
    ARGS
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt
    #  DEPENDS bbtk bbi 
    )
  
  ADD_CUSTOM_TARGET(doxygen_${NAME} ALL
    DEPENDS  ${DOXYGEN_OUTPUT}/${DOXYGEN_HTML_OUTPUT}/index.html
    )
  
  INSTALL(
    DIRECTORY
    ${DOXYGEN_OUTPUT}/${DOXYGEN_HTML_OUTPUT}
    DESTINATION
    ${${PROJECT_NAME}_DOXYGEN_INSTALL_PATH}/${DOC_RELATIVE_INSTALL_PATH}
    )
  #--------------------------------------------------------------------------
#message("user build doxygen , doxygen output = ${DOXYGEN_OUTPUT}")
ENDMACRO(USER_BUILD_DOXYGEN_DOC)
