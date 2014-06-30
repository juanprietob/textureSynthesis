
#----------------------------------------------------------------------------
# SET THE NAME OF YOUR LIBRARY
SET ( SOURCEGROUP_NAME   ANN  )
#----------------------------------------------------------------------------

 
FILE(GLOB ${SOURCEGROUP_NAME}_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCEGROUP_NAME}/*.h)
  # OR MANUALLY LIST YOUR HEADERS WITH NEXT COMMAND
  #  SET ( ${LIBRARY_NAME}_HEADERS
  #
  #    )
  #----------------------------------------------------------------------------

  #----------------------------------------------------------------------------
  # LIBRARY SOURCES (TO BE COMPILED)
  # EITHER LIST ALL .cxx, *.cpp, *.cc IN CURRENT DIR USING NEXT LINE:
FILE(GLOB ${SOURCEGROUP_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCEGROUP_NAME}/*.cxx)

IF(WIN32)
	SOURCE_GROUP("Source Files\\${SOURCEGROUP_NAME}" FILES ${${SOURCEGROUP_NAME}_SOURCES})
	SOURCE_GROUP("Header Files\\${SOURCEGROUP_NAME}" FILES ${${SOURCEGROUP_NAME}_HEADERS})
ELSE(WIN32)
	SOURCE_GROUP("Source Files/${SOURCEGROUP_NAME}" FILES ${${SOURCEGROUP_NAME}_SOURCES})
	SOURCE_GROUP("Header Files/${SOURCEGROUP_NAME}" FILES ${${SOURCEGROUP_NAME}_HEADERS})	
ENDIF(WIN32)

SET(${LIBRARY_NAME}_SOURCES ${${LIBRARY_NAME}_SOURCES} ${${SOURCEGROUP_NAME}_SOURCES})
SET(${LIBRARY_NAME}_HEADERS ${${LIBRARY_NAME}_HEADERS} ${${SOURCEGROUP_NAME}_HEADERS})


