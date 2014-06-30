

MACRO(USER_BUILD_HTML_DOC_FROM_LATEX CONFERENCEFOLDER TEX_FILE)
  # Need tth
  SET(USE_TTH ON CACHE BOOL "" FORCE)
  # Construct absolute build path
  SET(BUILD_PATH ${${PROJECT_NAME}_DOC_BUILD_PATH}/${OUTPUT_REL_PATH})
  #------------------------------------------------------------------------- 
  #IF(WIN32)
	#	SET(TTH_EXECUTABLE "${TTH_EXECUTABLE}/tth.exe")
	#ENDIF(WIN32)
	
	#message("MACRO(USER_BUILD_HTML_DOC_FROM_LATEX TEX_FILE OUTPUT_REL_PATH) TTH_EXECUTABLE ${TTH_EXECUTABLE}")
  ADD_CUSTOM_COMMAND(
    OUTPUT ${BUILD_PATH}/${TEX_FILE}.html
    COMMAND 
    #    tth 
    ${TTH_EXECUTABLE}
    ARGS
    -e2 ${BUILD_PATH}/${TEX_FILE}.tex 2>tth.log
 #   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TEX_FILE}.pdf
    )
  ADD_CUSTOM_TARGET(${TEX_FILE}.html ALL
    DEPENDS ${BUILD_PATH}/${TEX_FILE}.html
    )
  # Install
  INSTALL( 
    FILES ${BUILD_PATH}/${TEX_FILE}.html
    DESTINATION ${${PROJECT_NAME}_DOC_INSTALL_PATH}/${OUTPUT_REL_PATH}
    )   
  #-------------------------------------------------------------------------
  
ENDMACRO(USER_BUILD_HTML_DOC_FROM_LATEX)
