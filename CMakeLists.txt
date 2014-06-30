# ---------------------------------
#
# Unless you are an experimented cmake user,
# have a look at :
# http://www.creatis.insa-lyon.fr/site/fr/CreatoosGettingStarted-v2.0.X
# before starting.
#
# Generated with creaNewProject, version of : 18/03/2010
# ---------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#MARK_AS_ADVANCED( FORCE CMAKE_BACKWARDS_COMPATIBILITY )

# for CMake 2.6 corrected behaviour (see "cmake --help-policy CMP0003")
IF(COMMAND cmake_policy AND ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)
  CMAKE_POLICY(SET CMP0003 NEW)
  CMAKE_POLICY(SET CMP0005 NEW)
  CMAKE_POLICY(SET CMP0011 NEW)
ENDIF(COMMAND cmake_policy AND ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 4)


###################################
PROJECT( creaTextureSynthesis )
###################################

#==================================
# The project version
SET(PROJECT_MAJOR_VERSION 1)
SET(PROJECT_MINOR_VERSION 0)
SET(PROJECT_BUILD_VERSION 0)

SET(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_BUILD_VERSION}")
SET(PROJECT_VERSION_DATE "11/02/2010")
#ADD_DEFINITIONS( -DPROJECT_VERSION=${PROJECT_VERSION} )
#ADD_DEFINITIONS( -DPROJECT_VERSION_DATE=${PROJECT_VERSION_DATE} )


#==================================
# Find crea (mandatory, to use macros)

#==================================

# USER! : Add here as many 'FIND_PACKAGE' blocks as *your* aplication requires
#FIND_PACKAGE(xxx REQUIRED)
#IF (xxx_FOUND)
#  INCLUDE(${xxx_USE_FILE})
#ENDIF(xxx_FOUND)

#==================================
# Do not allow to build inside the source tree
IF("${PROJECT_BINARY_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
   MESSAGE(FATAL_ERROR "Building in the source tree is not allowed !! Quit; remove the file 'CMakeCache.txt' and the folder 'CMakeFiles' and build outside the sources")
ENDIF("${PROJECT_BINARY_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
#==================================

#==================================
# Libraries/tools used

# USER! : Note : Set USE_CREA to ON
#  if you need to LINK against crea
#  (and not only use its macros)
SET(USE_CREA      OFF)

SET(USE_GDCM      OFF)
SET(USE_GDCM_VTK  OFF)
SET(USE_GDCM2     OFF)
SET(USE_WXWIDGETS OFF)
SET(USE_KWWIDGETS OFF)
SET(USE_VTK       ON)
SET(USE_ITK       ON)
SET(USE_BOOST     OFF)
SET(USE_QT4       OFF)


#FIND_PACKAGE(creaMaracasVisu REQUIRED)
#IF(creaMaracasVisu_FOUND)
#  INCLUDE(${creaMaracasVisu_USE_FILE})
#ENDIF(creaMaracasVisu_FOUND)

#FIND_PACKAGE(creaANN REQUIRED)
#IF(creaANN_FOUND)
#  INCLUDE(${creaANN_USE_FILE})
#ENDIF(creaANN_FOUND)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ENDIF(VTK_FOUND)


# USER! : Note : Set USE_DOXYGEN to ON
#  if you think you will ask, some day,
#  for Doxygen generation
#EED SET(USE_DOXYGEN   OFF)

# Note : Set USE_LATEX to ON, USE_TTH to ON
#  if you think you will ask, some day,
#  for doc generation using your own latex documents
#EED SET(USE_LATEX     OFF)
#EED SET(USE_TTH       OFF)
INCLUDE(cmake/UserMacros.cmake)
#CREA_FIND_AND_USE_LIBRARIES()
#==================================

#==================================
# Where to put executables and libs
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
MARK_AS_ADVANCED(
  CMAKE_BACKWARDS_COMPATIBILITY
  EXECUTABLE_OUTPUT_PATH
  LIBRARY_OUTPUT_PATH
  )
#==================================

IF(WIN32)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/win32)
  LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/win32)

  LINK_DIRECTORIES(${creaANN_DIR}/Debug)
  LINK_DIRECTORIES(${creaANN_DIR}/Release)
ENDIF(WIN32)

#================================== 

#==================================

  SET(LIBNAME $(creaTextureSynthesis))
  
  INCLUDE_DIRECTORIES(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/lib/ann
    ${PROJECT_SOURCE_DIR}/lib/ann/ANN
    ${PROJECT_SOURCE_DIR}/lib/ITKPCA
    ${PROJECT_SOURCE_DIR}/lib/TextureSynthesis
    ${PROJECT_SOURCE_DIR}/lib/ITKFilters
    ${PROJECT_SOURCE_DIR}/lib/ITKANNConnector
    #${PROJECT_SOURCE_DIR}/lib/ITKStats

	#${creaANN_INCLUDE_DIRS}
  )
  
 # USER! : Add here the directories holding the .h you need   
 # INCLUDE_DIRECTORIES(appli/_YOUR_OWN_APPLI_SOURCE_DIRECTORY_)  
 # INCLUDE_DIRECTORIES(lib/_YOUR_OWN_LIBRARY_SOURCE_DIRECTORY_)  

#==================================


#==================================
# Where to generate documentation
# (desperate try; JPR)

SET(BBTK_DOC_BUILD_PATH 
     ${PROJECT_BINARY_DIR}/doc
   )
#==================================
 # USER! : Hands off!
# ADD_SUBDIRECTORY
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(appli)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(install)
#==================================
