

MACRO(USER_BUILD_LATEX_DOC NUM CONFERENCEFOLDER TEX_FILE)
  # Need LaTeX
  SET(USE_LATEX ON CACHE BOOL "" FORCE)
  # Construct absolute build path
  SET(BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CONFERENCEFOLDER})
  #MESSAGE(STATUS "Tex file : ${TEX_FILE}")
  #---------------------------------------------------------------------------

  CREA_CPDIR(${CMAKE_CURRENT_SOURCE_DIR}/${CONFERENCEFOLDER} ${BUILD_PATH})

  # Have to run latex twice to get references, toc, etc.
 #message("ADD_CUSTOM_COMMAND(OUTPUT ${BUILD_PATH}/${TEX_FILE}.aux   MACRO(USER_BUILD_LATEX_DOC")
ADD_CUSTOM_COMMAND(OUTPUT 
    ${BUILD_PATH}/${TEX_FILE}.pdf    

    COMMAND 
    # latex
    cd ${BUILD_PATH} && ${LATEX_COMPILER}
    ARGS
    ${BUILD_PATH}/${TEX_FILE}   

    COMMAND 
    # bibtex
    cd ${BUILD_PATH} && ${BIBTEX_COMPILER}
    ARGS
    ${BUILD_PATH}/${TEX_FILE}

    COMMAND 
    # latex
    cd ${BUILD_PATH} && ${LATEX_COMPILER}
    ARGS
    ${BUILD_PATH}/${TEX_FILE}   

    COMMAND 
    # latex
    cd ${BUILD_PATH} && ${DVIPDF_CONVERTER}
    ARGS
    ${BUILD_PATH}/${TEX_FILE}
    )    


add_custom_target(${CONFERENCEFOLDER}${TEX_FILE}.pdf ALL
		  DEPENDS ${BUILD_PATH}/${TEX_FILE}.pdf)


ADD_CUSTOM_COMMAND(OUTPUT 
    ${BUILD_PATH}/${TEX_FILE}.html

    COMMAND 
    # latex
    cd ${BUILD_PATH} && ${LATEX2HTML_CONVERTER}
    ARGS
    -antialias -white -notransparent ${BUILD_PATH}/${TEX_FILE}
)

add_custom_target(${CONFERENCEFOLDER}${TEX_FILE}.html ALL
		  DEPENDS ${BUILD_PATH}/${TEX_FILE}.html)
  
#message(status "++++++++++++++++++++++++++ ${${PROJECT_NAME}_DOC_INSTALL_PATH}/${OUTPUT_REL_PATH}")

SET(ARTICLE_LINKS
      "${ARTICLE_LINKS}
       <br>${NUM}. <a href=\"../UserDocumentation/${CONFERENCEFOLDER}/${TEX_FILE}/index.html\">${TEX_FILE}</a>
       <a href=\"../UserDocumentation/${CONFERENCEFOLDER}/${TEX_FILE}.pdf\">PDF File</a><br>"
   )

ENDMACRO(USER_BUILD_LATEX_DOC)
