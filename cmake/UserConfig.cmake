

#-----------------------------------------------------------------------------
# store documentation and scripts **RELATIVE** paths  
# from build tree root or install tree root
# (different on win/lin)
IF(WIN32)
  # A trick to install in root install dir (problem when empty path given)
  SET(${PROJECT_NAME}_SHARE_REL_PATH "bin/..")
  SET(${PROJECT_NAME}_DOC_REL_PATH doc) 
ELSE(WIN32)
  SET(${PROJECT_NAME}_SHARE_REL_PATH share)
  SET(${PROJECT_NAME}_DOC_REL_PATH doc)  
ENDIF(WIN32)   

MESSAGE(STATUS "* SHARE_REL_PATH   =${${PROJECT_NAME}_SHARE_REL_PATH}")
MESSAGE(STATUS "* DOC_REL_PATH     =${${PROJECT_NAME}_DOC_REL_PATH}")
MESSAGE(STATUS "* BBS_REL_PATH     =${${PROJECT_NAME}_BBS_REL_PATH}")
MESSAGE(STATUS "* DATA_REL_PATH    =${${PROJECT_NAME}_DATA_REL_PATH}")
MESSAGE(STATUS "===============================================")

#-----------------------------------------------------------------------------
