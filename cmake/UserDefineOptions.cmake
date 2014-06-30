#-----------------------------------------------------------------------------
# messages compilation options 
OPTION ( BUILD_${PROJECT_NAME}_DOC "Build documentation for ${PROJECT_NAME}." OFF)
SWITCH_ON_IF_BUILD_ALL(BUILD_${PROJECT_NAME}_DOC)
#-----------------------------------------------------------------------------
IF(BUILD_${PROJECT_NAME}_DOC)

  OPTION(BUILD_${PROJECT_NAME}_DOC_PACKAGE 
    "Build ${PROJECT_NAME} packages documentation (bbi+dot)."       ON)
  OPTION(BUILD_${PROJECT_NAME}_DOC_DOXYGEN 
    "Build doxygen documentation (doxygen)."             ON)
  OPTION(BUILD_${PROJECT_NAME}_DOC_GUIDES_PDF 
    "Build ${PROJECT_NAME} Guides', PDF format (latex)."            ON)
  OPTION(BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML 
    "Build ${PROJECT_NAME} Guides', HTML format (latex+tth)."       ON)

ELSE(BUILD_${PROJECT_NAME}_DOC)
  
  IF(BUILD_${PROJECT_NAME}_DOC_PACKAGE)
    SET(BUILD_${PROJECT_NAME}_DOC_PACKAGE OFF CACHE BOOL 
      "Build ${PROJECT_NAME} packages documentation (bbdoc+dot)."	FORCE)
  ENDIF(BUILD_${PROJECT_NAME}_DOC_PACKAGE)
  IF(BUILD_${PROJECT_NAME}_DOC_DOXYGEN)
    SET(BUILD_${PROJECT_NAME}_DOC_DOXYGEN OFF CACHE BOOL 
      "Build doxygen documentation. (doxygen)"		FORCE)
  ENDIF(BUILD_${PROJECT_NAME}_DOC_DOXYGEN)
  
  IF(BUILD_${PROJECT_NAME}_DOC_GUIDES_PDF)
    SET(BUILD_${PROJECT_NAME}_DOC_GUIDES_PDF OFF CACHE BOOL 
      "Build ${PROJECT_NAME} Guides', PDF format (latex)."	FORCE)
  ENDIF(BUILD_${PROJECT_NAME}_DOC_GUIDES_PDF)
  
  IF(BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML)
    SET(BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML OFF CACHE BOOL 
      "Build ${PROJECT_NAME} Guides', HTML format (latex tth)."		FORCE)
  ENDIF(BUILD_${PROJECT_NAME}_DOC_GUIDES_HTML)
  
ENDIF(BUILD_${PROJECT_NAME}_DOC)
#-----------------------------------------------------------------------------
