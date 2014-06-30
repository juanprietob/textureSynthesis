

#-----------------------------------------------------------------------------
IF(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_DOXYGEN)
  INCLUDE(../cmake/UserBuildDoxygenDoc.cmake)
  ADD_SUBDIRECTORY(UserDoxygen)
ENDIF(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_DOXYGEN)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
IF(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_GUIDES_PDF)
  INCLUDE(../cmake/UserBuildLatexDoc.cmake)
ELSE(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML)
  MACRO(USER_BUILD_LATEX_DOC TEX_FILE)
  ENDMACRO(USER_BUILD_LATEX_DOC)
ENDIF(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_GUIDES_PDF)
#-----------------------------------------------------------------------------


#-----------------------------------------------------------------------------
IF(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML)
  INCLUDE(../cmake/UserBuildHtmlDocFromLatex.cmake)
ELSE(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML)
  MACRO(USER_BUILD_HTML_DOC_FROM_LATEX SOURCE)
  ENDMACRO(USER_BUILD_HTML_DOC_FROM_LATEX)
ENDIF(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML)
#-----------------------------------------------------------------------------


#-----------------------------------------------------------------------------
IF(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_GUIDES_PDF OR BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML)  
  ADD_SUBDIRECTORY(UserDocumentation)  
ENDIF(BUILD_${PROJECT_NAME}_DOC AND BUILD_${PROJECT_NAME}_DOC_GUIDES_PDF OR BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML)
#-----------------------------------------------------------------------------
# Web site 
IF(BUILD_${PROJECT_NAME}_DOC)
  # Configure main page index.html for build tree
  # Configure main page index-no-frame.html for build tree

  # Recurse it bbtkWebSite dir
  #ADD_SUBDIRECTORY(template_WebSite)



ENDIF(BUILD_${PROJECT_NAME}_DOC)
#-----------------------------------------------------------------------------

