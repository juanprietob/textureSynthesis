MESSAGE(STATUS "")
MESSAGE(STATUS "=======================================")
MESSAGE(STATUS "Configuring user doxygen doc")

CONFIGURE_FILE(
  ${CMAKE_CURRENT_BINARY_DIR}/../UserDocumentation/UserDocumentationLinks.txt
  ${CMAKE_CURRENT_BINARY_DIR}/UserDocumentationLinks.txt
  @ONLY IMMEDIATE
  )


SET(EXTERNALINHERITED itkImageToImageFilter.h
		      itkImageToVectorImageFilter.h
		      itkProcessObject.h
                      itkannconnector.h
		      itkImageToListAdaptor.h
		      itkListSampleBase.h
		      itkSampleAlgorithmBase.h
   )


GET_PROPERTY(INC_DIRS DIRECTORY PROPERTY INCLUDE_DIRECTORIES)

#message(status "*********** EXTERNALINHERITED ${EXTERNALINHERITED}")
foreach(fileinherited ${EXTERNALINHERITED})
	FIND_FILE(F0_${fileinherited} ${fileinherited}
		  HINTS ${INC_DIRS})

	SET(ALL_INHERITED "${F0_${fileinherited}} ${ALL_INHERITED}")	
endforeach(fileinherited)

#message(status "*********** ALL_INHERITED ${ALL_INHERITED}")



SET(INPUT 
  ${CMAKE_CURRENT_BINARY_DIR}/UserDocumentationLinks.txt
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_SOURCE_DIR}/appli
  ${ALL_INHERITED}
  )

INCLUDE(../../cmake/UserBuildDoxygenDoc.cmake)

USER_BUILD_DOXYGEN_DOC(
  "${PROJECT_NAME}" 
  "${INPUT}" 
  "${PROJECT_NAME}" 
  "${DOXYGEN_DOC_PREDEFINED}"
  )

MESSAGE(STATUS "=======================================")
