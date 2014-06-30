#----------------------------------------------------------------------------
# USER! : SET THE NAME OF YOUR EXECUTABLE
# Replace 'MyExe' by the name you want to give your executable.
# (a good policy is to give the executable the same name that the directory)

#########################
SET ( EXE_NAME   testRGBImageToVector )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  ${creaANN_LIBRARIES}
  ITKFilters
  ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...) 
  
  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )

#########################
SET ( EXE_NAME   testRandomImageGenerator )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)

SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  ${creaANN_LIBRARIES}
  ITKFilters
  ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )
#----------------------------------------------------------------------------


#########################
SET ( EXE_NAME   testVectorFeaturesGenerator )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)

SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  ${creaANN_LIBRARIES}
  ITKFilters
  ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
#CREA_ADD_EXECUTABLE( ${EXE_NAME} )
#----------------------------------------------------------------------------


#########################
SET ( EXE_NAME   testSampleFeaturesGenerator )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)

SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  ${creaANN_LIBRARIES}
  ITKFilters

  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )


#########################
SET ( EXE_NAME   testRandomImageGenerator3D )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)

SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
      ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}
  ITKFilters
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )




#########################
SET ( EXE_NAME   testITKFilters )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)

SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}
  #    ITKFilters
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )


#########################
SET ( EXE_NAME   testAND )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)

SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}
  #    ITKFilters
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )


#########################
SET ( EXE_NAME   testImageThreshold )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  #ITKFilters
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )


#########################
SET ( EXE_NAME   testImageCrop )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  #ITKFilters
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )

#########################
SET ( EXE_NAME   testHistogramMatching )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
  #    ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  ITKFilters
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )


#########################
SET ( EXE_NAME   testViewVectorImage )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
      ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  ITKFilters
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )

#########################
SET ( EXE_NAME   testX2UTextured )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
      ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  ITKFilters
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )


#########################
SET ( EXE_NAME   testAppendImages )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  ITKFilters
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )

#########################
SET ( EXE_NAME   testCarveTexture )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  ITKFilters
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )

#########################
SET ( EXE_NAME   testResampleImage )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  #ITKFilters
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )


#########################
SET ( EXE_NAME   testVTKFilters )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
      ${VTK_LIBRARIES}
  #    ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}

  #${creaANN_LIBRARIES}
  #ITKANNConnector
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )

#----------------------------------------------------------------------------
# USER! : SET THE NAME OF YOUR EXECUTABLE
# Replace 'MyExe' by the name you want to give your executable.
# (a good policy is to give the executable the same name that the directory)

#########################
SET ( EXE_NAME   testMedianFilter )
#########################
SET(${EXE_NAME}_SOURCES ${EXE_NAME}.cxx)
SET ( ${EXE_NAME}_LINK_LIBRARIES
  #    ${crea_LIBRARIES}
  #    ${WXWIDGETS_LIBRARIES}
  #    ${KWWidgets_LIBRARIES}
  #    ${VTK_LIBRARIES}
      ${ITK_LIBRARIES}
  #    ${GDCM_LIBRARIES}
  #    ${BOOST_LIBRARIES}
  # USER! : Add here those agmonst the various (?) PROJECT LIBRARIES
  # you need for the current executable
  # (If you created only one Library, don't forget it !...)

  )
CREA_ADD_EXECUTABLE( ${EXE_NAME} )
