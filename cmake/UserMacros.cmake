

#========================================================================
# MACRO WHICH COPIES ALL IMAGES (png, jpg, gif) 
# FROM SOURCE TREE TO DOC BUILD TREE
# AND INSTALL THEM IN BBTK DOC TREE
MACRO(USER_DOC_INSTALL_IMAGES DOC_RELATIVE_INSTALL_PATH)
  MESSAGE(STATUS "* Copying png,jpg,tif images in ${BBTK_DOC_BUILD_PATH}/${DOC_RELATIVE_INSTALL_PATH}")
  FILE(GLOB PNG_IMAGES "*.png")
  FILE(GLOB JPG_IMAGES "*.jpg")
  FILE(GLOB TIF_IMAGES "*.tif")
  SET(IMAGES
    ${PNG_IMAGES}
    ${JPG_IMAGES}
    ${TIF_IMAGES}
    )
#    MESSAGE(ERROR ${IMAGES} )
  FOREACH(image ${IMAGES})  
    GET_FILENAME_COMPONENT(filename "${image}" NAME)
    CONFIGURE_FILE(
      ${image}
       ${${PROJECT_NAME}_DOC_BUILD_PATH}/${DOC_RELATIVE_INSTALL_PATH}/${filename}
      COPYONLY
      ) 
      
    #  IF (WIN32)
#	CONFIGURE_FILE(
#          ${image}
#          ${CMAKE_CURRENT_BINARY_DIR}/${filename}
#          COPYONLY
#	  )
#      ENDIF(WIN32)
    # MESSAGE(ERROR "${image} (${filename}) in :  ${PROJECT_BINARY_DIR}/${BBTK_DOC_INSTALL_PATH}/${DOC_RELATIVE_INSTALL_PATH}/${filename}")
  ENDFOREACH(image)
  INSTALL( 
      FILES ${IMAGES} 
      DESTINATION ${${PROJECT_NAME}_DOC_INSTALL_PATH}/${DOC_RELATIVE_INSTALL_PATH}
    )  
ENDMACRO(USER_DOC_INSTALL_IMAGES)


MACRO(CREA_PREVENT_IN_SOURCE_BUILD)
  IF(NOT CREA_ALLOW_IN_SOURCE_BUILD)
    #------------------------------------------------------------------------
    # Building in the source tree is forbidden ! 
    # except if CREA_ALLOW_IN_SOURCE_BUILD is set, 
    # e.g. for packaging with rpmbuild
    IF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
      MESSAGE(FATAL_ERROR "Building in the source tree is not allowed !! Quit; remove the file 'CMakeCache.txt' and the folder 'CMakeFiles' and build outside the sources")
    ENDIF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
    #------------------------------------------------------------------------
  ENDIF(NOT CREA_ALLOW_IN_SOURCE_BUILD)
ENDMACRO(CREA_PREVENT_IN_SOURCE_BUILD)



#========================================================================
# Creates a directory if it does not exist
MACRO(CREA_MKDIR DIR)
  IF(NOT IS_DIRECTORY ${DIR})
    MESSAGE(STATUS "* Creating directory '${DIR}'")
    FILE(MAKE_DIRECTORY ${DIR})
  ENDIF(NOT IS_DIRECTORY ${DIR})
ENDMACRO(CREA_MKDIR)
#========================================================================

#========================================================================
# COPIES A DIRECTORY EXCLUDING CVS ENTRIES
MACRO(CREA_CPDIR SOURCE DEST)
  FILE(GLOB_RECURSE NAMES RELATIVE ${SOURCE} ${SOURCE}/*)
  FOREACH( NAME  ${NAMES})
    STRING(SUBSTRING ${NAME} 0 3 subStrTmp )
    IF( NOT subStrTmp STREQUAL "../" )
      STRING(REGEX MATCH "CVS" CVSSUBS_FOUND ${NAME} )
      IF( NOT CVSSUBS_FOUND STREQUAL "CVS" )
	CONFIGURE_FILE(
	  ${SOURCE}/${NAME} 
	  ${DEST}/${NAME}
	  COPYONLY )
      ENDIF( NOT CVSSUBS_FOUND STREQUAL "CVS" )
    ENDIF( NOT subStrTmp STREQUAL "../" )
  ENDFOREACH(NAME)
ENDMACRO(CREA_CPDIR)
#========================================================================

# Creates and installs a library
# Uses CREA_DYNAMIC_LIBRARY_EXPORT and CREA_INSTALL_LIBRARY
# hence the variables to be set for these are needed 
# Additionnaly one needs to set :
# ${LIBRARY_NAME}_HEADERS : the list of headers to install
# ${LIBRARY_NAME}_SOURCES : the list of files to compile to build the lib
# ${LIBRARY_NAME}_LINK_LIBRARIES : the list of libs to link with
MACRO(CREA_ADD_LIBRARY LIBRARY_NAME)

  IF(CREA_VERBOSE_CMAKE)
    MESSAGE(STATUS "===============================================")
    MESSAGE(STATUS "Configuring library ${LIBRARY_NAME}")
    MESSAGE(STATUS "   Headers  : ${${LIBRARY_NAME}_HEADERS}")
    MESSAGE(STATUS "   Sources  : ${${LIBRARY_NAME}_SOURCES}")
    MESSAGE(STATUS "   Link libs: ${${LIBRARY_NAME}_LINK_LIBRARIES}")
    MESSAGE(STATUS "===============================================")
  ENDIF(CREA_VERBOSE_CMAKE)


  # MANAGE SHARED LIB
  CREA_DYNAMIC_LIBRARY_EXPORT_OPTION(${LIBRARY_NAME})

  # CREATE THE TARGET
  ADD_LIBRARY(${LIBRARY_NAME} ${${LIBRARY_NAME}_SHARED} ${${LIBRARY_NAME}_SOURCES})

  # LINK
  TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${${LIBRARY_NAME}_LINK_LIBRARIES})

  # INSTALLS LIBRARY FOR CMAKE
  CREA_INSTALL_LIBRARY_FOR_CMAKE(${LIBRARY_NAME})

  # INSTALLS LIBRARY
  INSTALL(
    FILES ${${LIBRARY_NAME}_HEADERS}
    DESTINATION ${${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS}
    )  

IF(WIN32)	
  INSTALL(
    TARGETS ${LIBRARY_NAME}
    DESTINATION bin)
ELSE(WIN32)
  INSTALL(
    TARGETS ${LIBRARY_NAME}
    DESTINATION ${CMAKE_CREA_LIB_PATH} )
ENDIF(WIN32)

#EED 01/July/2011
IF (UNIX)
  add_definitions(-fPIC)
ENDIF(UNIX)


ENDMACRO(CREA_ADD_LIBRARY)


# Manages the shared library creation/use 
# * Creates an option ${LIBRARY_NAME}_BUILD_SHARED 
# * Generates the file ${LIBRARY_NAME}_EXPORT.h which 
#   defines the symbols ${LIBRARY_NAME}_EXPORT and ${LIBRARY_NAME}_CDECL
#   to be used in exported classes/functions declarations
MACRO(CREA_DYNAMIC_LIBRARY_EXPORT_OPTION LIBRARY_NAME)

  # STATIC OR DYNAMIC (SHARED) ? 
  OPTION( ${LIBRARY_NAME}_BUILD_SHARED 
  #JPR 8 Avr 2011 ON-> OFF, to avoid troubles with unaware users
    "Build ${LIBRARY_NAME} as a shared library ( i.e. dynamic) ?" OFF)
  IF (${LIBRARY_NAME}_BUILD_SHARED)
    SET(${LIBRARY_NAME}_SHARED SHARED)
    CREA_DEFINE(${LIBRARY_NAME}_BUILD_SHARED)
  ENDIF(${LIBRARY_NAME}_BUILD_SHARED)
  
  CREA_DEFINE(${LIBRARY_NAME}_EXPORT_SYMBOLS)

  # ADDS CURRENT BINARY DIR TO INCLUDE DIRS
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

  # CONFIGURES ${LIBRARY_NAME}_EXPORT.h
  CONFIGURE_FILE(
    ${CREA_CMAKE_DIR}/LIBRARY_NAME_EXPORT.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_EXPORT.h
    @ONLY IMMEDIATE
    )
  # ADDS IT TO THE LIST OF HEADERS
  SET(${LIBRARY_NAME}_HEADERS
    ${${LIBRARY_NAME}_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_EXPORT.h
    )
  

ENDMACRO(CREA_DYNAMIC_LIBRARY_EXPORT_OPTION LIBRARY_NAME)


# Manages the shared library creation/use 
# * Generates the file ${LIBRARY_NAME}_EXPORT.h which 
#   defines the symbols ${LIBRARY_NAME}_EXPORT and ${LIBRARY_NAME}_CDECL
#   to be used in exported classes/functions declarations
MACRO(CREA_DYNAMIC_LIBRARY_EXPORT LIBRARY_NAME)

  CREA_DEFINE(${LIBRARY_NAME}_BUILD_SHARED)
  CREA_DEFINE(${LIBRARY_NAME}_EXPORT_SYMBOLS)

  # ADDS CURRENT BINARY DIR TO INCLUDE DIRS
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

  # CONFIGURES ${LIBRARY_NAME}_EXPORT.h
  SET(LIBRARY_NAME ${LIBRARY_NAME})
  CONFIGURE_FILE(
    ${CREA_CMAKE_DIR}/LIBRARY_NAME_EXPORT.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_EXPORT.h
    @ONLY IMMEDIATE
    )
  # ADDS IT TO THE LIST OF HEADERS
  SET(${LIBRARY_NAME}_HEADERS
    ${${LIBRARY_NAME}_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_EXPORT.h
    )

ENDMACRO(CREA_DYNAMIC_LIBRARY_EXPORT LIBRARY_NAME)


#-----------------------------------------------------------------------------
# MACRO TO ADD A COMPILER DEFINITION WHICH ADDS ALSO THE DEF 
# INTO DOXYGEN PREDEFINED VAR 
# (OTHERWISE DOXYGEN WOULD NOT PARSE THE FILES BLOCKED)
MACRO(CREA_DEFINE DEF)
  MESSAGE(STATUS "* Added definition -D${DEF}")
  ADD_DEFINITIONS( -D${DEF} )
  SET(CREA_DEFINITIONS
    ${CREA_DEFINITIONS};${DEF}
    CACHE 
    STRING 
    ""
    FORCE)
  MARK_AS_ADVANCED(CREA_DEFINITIONS)
ENDMACRO(CREA_DEFINE)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# MACRO TO ADD A COMPILER DEFINITION WITH VALUE WHICH ADDS ALSO THE DEF 
# INTO DOXYGEN PREDEFINED VAR 
# (OTHERWISE DOXYGEN WOULD NOT PARSE THE FILES BLOCKED)
MACRO(CREA_DEFINE_WITH_VAL DEF VAL)
  MESSAGE(STATUS "* Added definition -D${DEF}=${VAL}")
  ADD_DEFINITIONS( -D${DEF}=${VAL} )
  SET(CREA_DEFINITIONS
    ${CREA_DEFINITIONS};${DEF}=${VAL}
    CACHE 
    STRING 
    ""
    FORCE)
  MARK_AS_ADVANCED(CREA_DEFINITIONS)
ENDMACRO(CREA_DEFINE_WITH_VAL)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
MACRO(CREA_DEFINE_OPERATING_SYSTEM )
  IF(APPLE)
   CREA_DEFINE(MACOSX)
  ELSE(APPLE)
    IF("${CMAKE_SYSTEM}" MATCHES "Linux")
      CREA_DEFINE(LINUX)
    ELSE("${CMAKE_SYSTEM}" MATCHES "Linux")
      IF(WIN32)
        CREA_DEFINE(WIN32)
      ELSE(WIN32)
        MESSAGE(FATAL_ERROR "${CMAKE_SYSTEM} unsupported ! Build at your own risks ! (remove this line from kernel/cmake/BBTKAddDefinitions.cmake)")
      ENDIF(WIN32)
    ENDIF("${CMAKE_SYSTEM}" MATCHES "Linux")
  ENDIF(APPLE) 


  IF (WIN32)
    SET(CMAKE_CREA_LIB_PATH bin)
  ELSE (WIN32)
    if( NOT APPLE )
      # check 64 bit
      if( ${CMAKE_SIZEOF_VOID_P} EQUAL 4 )
	 set( HAVE_64_BIT 0 )
	 SET(CMAKE_CREA_LIB_PATH lib/creatools)
      else( ${CMAKE_SIZEOF_VOID_P}EQUAL 4 )
	 set( HAVE_64_BIT 1 )
	 SET(CMAKE_CREA_LIB_PATH lib64/creatools)
      endif( ${CMAKE_SIZEOF_VOID_P} EQUAL 4 )
    else ( NOT APPLE )
	 SET(CMAKE_CREA_LIB_PATH lib/creatools)

      if( ${CMAKE_SIZEOF_VOID_P} EQUAL 4 )
	 message("EED crea definitions  --------APPLE------------ 64 0")
      else( ${CMAKE_SIZEOF_VOID_P}EQUAL 4 )
	 message("EED crea definitions  --------APPLE------------ 64 1")
      endif( ${CMAKE_SIZEOF_VOID_P} EQUAL 4 )


    endif( NOT APPLE )
  ENDIF(WIN32)


ENDMACRO(CREA_DEFINE_OPERATING_SYSTEM)
#-----------------------------------------------------------------------------

#Generates the cmake commands to build and install the executable EXE_NAME.
# ${EXE_NAME}_SOURCES        must contain the list of files to compile 
#                            to build the executable.
# ${EXE_NAME}_LINK_LIBRARIES must contain the list of libraries to link with
#
# On Windows:
# if ${EXE_NAME}_HAS_GUI is set to TRUE then a Win32 application is generated
# if ${EXE_NAME}_CONSOLE is set to TRUE then the application will have a console attached.
MACRO(CREA_ADD_EXECUTABLE EXE_NAME)

  IF(CREA_VERBOSE_CMAKE)
    MESSAGE(STATUS "===============================================")
    MESSAGE(STATUS "Configuring executable ${EXE_NAME}")
    MESSAGE(STATUS "   Sources  : ${${EXE_NAME}_SOURCES}")
    MESSAGE(STATUS "   Link libs: ${${EXE_NAME}_LINK_LIBRARIES}")
    IF(WIN32)
      MESSAGE(STATUS "   Win32 app: ${${EXE_NAME}_HAS_GUI}")
      MESSAGE(STATUS "   Console  : ${${EXE_NAME}_CONSOLE}")
    ENDIF(WIN32)
    MESSAGE(STATUS "===============================================")
  ENDIF(CREA_VERBOSE_CMAKE)

  IF(WIN32 AND ${EXE_NAME}_HAS_GUI )
    ADD_EXECUTABLE(${EXE_NAME} WIN32 ${${EXE_NAME}_SOURCES})
    IF( ${${EXE_NAME}_CONSOLE} )
      SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES LINK_FLAGS /subsystem:console )
    ENDIF( ${${EXE_NAME}_CONSOLE} )
  ELSE(WIN32 AND ${EXE_NAME}_HAS_GUI )
    IF(${EXE_NAME}_HAS_GUI AND APPLE)
      ADD_EXECUTABLE(${EXE_NAME} MACOSX_BUNDLE ${${EXE_NAME}_SOURCES})
    ELSE(${EXE_NAME}_HAS_GUI AND APPLE)
      ADD_EXECUTABLE(${EXE_NAME}  ${${EXE_NAME}_SOURCES})
    ENDIF(${EXE_NAME}_HAS_GUI AND APPLE)
  ENDIF(WIN32 AND ${EXE_NAME}_HAS_GUI )
  
  TARGET_LINK_LIBRARIES(${EXE_NAME} ${${EXE_NAME}_LINK_LIBRARIES})

  INSTALL_TARGETS(/bin/ ${EXE_NAME})

ENDMACRO(CREA_ADD_EXECUTABLE)



#-----------------------------------------------------------------------------
# Macro CREA_ADVANCED_INSTALL_LIBRARY_FOR_CMAKE
# Creates and install the cmake files which allow 
# to find the library by the cmake command FIND_PACKAGE.
#
# Inputs :
# --------
#  LIBRARY_NAME : name of the library to find 
#
# The following variables **MUST** have been set previously :
#
# * ${LIBRARY_NAME}_BUILD_TREE_RELATIVE_INCLUDE_PATHS
#    The list of include paths 
#    when someone uses a *BUILD TREE* version of ${LIBRARY_NAME}.
#    NB : 
#     THE PATHS *MUST BE RELATIVE* TO THE ROOT DIR OF THE PROJECT **SOURCES** !
#    Assume your project architecture is :
#     install/ : directory in which the macro is invoked
#     src/part1/include/ : first include dir
#     src/part2/include/ : second include dir
#    Then you should set the var with :
#    SET(${LIBRARY_NAME}_BUILD_TREE_RELATIVE_INCLUDE_PATHS
#     src/part1/include
#     src/part2/include )
#    Which will result in actual include paths :
#     ${PROJECT_SOURCE_DIR}/src/part1/include;
#     ${PROJECT_SOURCE_DIR}/src/part2/include
# * ${LIBRARY_NAME}_BUILD_TREE_RELATIVE_LIBRARY_PATHS
#    Like the previous var but for the library paths.
#    NB : 
#     THE PATHS *MUST BE RELATIVE* TO THE ROOT DIR OF THE **BUILD TREE** 
#     THAT IS POINT TO THE FOLDERS WHERE THE LIBS WILL BE BUILD
#    Assume that your project architecture is :
#     src/part1/src/ : first source dir, in which the lib 'part1' is built
#     src/part2/src/ : first source dir, in which the lib 'part2' is built
#    Then you should set the var with 
#    SET(${LIBRARY_NAME}_BUILD_TREE_RELATIVE_LIBRARY_PATHS
#     src/part1/src
#     src/part2/src
#     )
#    Which will result in actual library paths :
#     ${PROJECT_BINARY_DIR}/src/part1/src
#     ${PROJECT_BINARY_DIR}/src/part2/src
# * ${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS
#    The list of include paths 
#    when someone uses an *INSTALLED* version of ${LIBRARY_NAME} 
#    The paths *MUST BE RELATIVE* to CMAKE_INSTALL_PREFIX
#  #    A typical example is "include/${LIBRARY_NAME}"
# * ${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS
#    Like the previous var but for library paths.
#    A typical example is "lib"
# * ${LIBRARY_NAME}_LIBRARIES
#    The list of libraries to link against when using ${LIBRARY_NAME}
#
# The following variables can be set optionally :
#
# * ${LIBRARY_NAME}_REQUIRED_C_FLAGS
# * ${LIBRARY_NAME}_REQUIRED_CXX_FLAGS
# * ${LIBRARY_NAME}_REQUIRED_LINK_FLAGS
# * ${LIBRARY_NAME}_MAJOR_VERSION
# * ${LIBRARY_NAME}_MINOR_VERSION
# * ${LIBRARY_NAME}_BUILD_VERSION
# * ${LIBRARY_NAME}_INSTALL_FOLDER : if set then install the generated files 
#   in CMAKE_INSTALL_PREFIX/lib/${LIBRARY_NAME}_INSTALL_FOLDER 
#   instead of CMAKE_INSTALL_PREFIX/lib/${LIBRARY_NAME}
#
# 
# To provide a user defined 
# couple of Config/Use file (in addition to the standard one) use :
#  SET( ${LIBRARY_NAME}_HAS_ADDITIONAL_CONFIG_FILE  TRUE )
# and store the *ABSOLUTE* paths to the additional files in the vars :
#  ${LIBRARY_NAME}_ADDITIONAL_CONFIG_FILE
#  ${LIBRARY_NAME}_ADDITIONAL_USE_FILE
# (e.g. ${CMAKE_CURRENT_SOURCE_DIR}/MyConfig.cmake)
#
# Outputs :
# --------
# At cmake run-time, the macro creates the following files 
# in the current dir of the build tree (where the macro is invoked) :
#  Use${LIBRARY_NAME}.cmake
#  ${LIBRARY_NAME}Config.cmake
#  ${LIBRARY_NAME}BuildSettings.cmake
# And if the vars ${LIBRARY_NAME}_ADDITIONAL_CONFIG_FILE and
# ${LIBRARY_NAME}_ADDITIONAL_USE_FILE are set, it also creates :
#  Additional${LIBRARY_NAME}Config.cmake
#  AdditionalUse${LIBRARY_NAME}.cmake
# 
# At install-time, the same files are installed 
# in CMAKE_INSTALL_PREFIX/lib/${LIBRARY_NAME}_INSTALL_FOLDER 
# and the file :
#  Find${LIBRARY_NAME}.cmake
# is installed in ${CMAKE_ROOT}/Modules/
#
#
#-----------------------------------------------------------------------------

MACRO(CREA_ADVANCED_INSTALL_LIBRARY_FOR_CMAKE LIBRARY_NAME)

  #---------------------------------------------------------------------------
  # Sets the common values to build tree and install tree configs
  SET(CILC_LIB_REQUIRED_C_FLAGS    ${${LIBRARY_NAME}_REQUIRED_C_FLAGS})
  SET(CILC_LIB_REQUIRED_CXX_FLAGS  ${${LIBRARY_NAME}_REQUIRED_CXX_FLAGS})
  SET(CILC_LIB_REQUIRED_LINK_FLAGS ${${LIBRARY_NAME}_REQUIRED_LINK_FLAGS})
  SET(CILC_LIB_MAJOR_VERSION       ${${LIBRARY_NAME}_MAJOR_VERSION})
  SET(CILC_LIB_MINOR_VERSION       ${${LIBRARY_NAME}_MINOR_VERSION})
  SET(CILC_LIB_BUILD_VERSION       ${${LIBRARY_NAME}_BUILD_VERSION})
  SET(CILC_LIB_VERSION             
    ${CILC_LIB_MAJOR_VERSION}.${CILC_LIB_MINOR_VERSION}.${CILC_LIB_BUILD_VERSION})
  SET(CILC_LIB_LIBRARIES ${${LIBRARY_NAME}_LIBRARIES})
  IF (${LIBRARY_NAME}_HAS_ADDITIONAL_CONFIG_FILE)
    SET(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE TRUE)
  ELSE (${LIBRARY_NAME}_HAS_ADDITIONAL_CONFIG_FILE)
    SET(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE FALSE)
  ENDIF (${LIBRARY_NAME}_HAS_ADDITIONAL_CONFIG_FILE)
  #---------------------------------------------------------------------------


  #---------------------------------------------------------------------------
  #-- BUILD TREE configuration 
  SET(CILC_BUILD_TREE_CONFIGURATION TRUE)
  # UseLIBRARY_NAME.cmake 
  CONFIGURE_FILE(
    ${CREA_CMAKE_DIR}/InstallLibraryForCMake_UseLibrary.cmake.in
    ${PROJECT_BINARY_DIR}/Use${LIBRARY_NAME}.cmake
    @ONLY IMMEDIATE
    )
  # LIBRARY_NAMEConfig.cmake 
  SET(CILC_LIB_RELATIVE_INCLUDE_PATHS 
    ${${LIBRARY_NAME}_BUILD_TREE_RELATIVE_INCLUDE_PATHS})
  SET(CILC_LIB_RELATIVE_LIBRARY_PATHS 
    ${${LIBRARY_NAME}_BUILD_TREE_RELATIVE_LIBRARY_PATHS})
  CONFIGURE_FILE(
    ${CREA_CMAKE_DIR}/InstallLibraryForCMake_LibraryConfig.cmake.in
    ${PROJECT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake
    @ONLY IMMEDIATE
    )
  # LIBRARY_NAMEBuildSettings.cmake :
  # Save the compiler settings so another project can import them.
  # FCY : DEPREDCATED FUNCTION FOR CMAKE 2.8
  IF( "${CMAKE_MINIMUM_REQUIRED_VERSION}" VERSION_LESS 2.7)
  INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
  CMAKE_EXPORT_BUILD_SETTINGS(${PROJECT_BINARY_DIR}/${LIBRARY_NAME}BuildSettings.cmake)
  ENDIF()
  # Additional Config and Use files
  IF(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE)
    CONFIGURE_FILE(
      ${${LIBRARY_NAME}_ADDITIONAL_CONFIG_FILE}
      ${PROJECT_BINARY_DIR}/Additional${LIBRARY_NAME}Config.cmake
      @ONLY IMMEDIATE
      )
    CONFIGURE_FILE(
      ${${LIBRARY_NAME}_ADDITIONAL_USE_FILE}
      ${PROJECT_BINARY_DIR}/AdditionalUse${LIBRARY_NAME}.cmake
      @ONLY IMMEDIATE
      )
  ENDIF(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE)
  #---------------------------------------------------------------------------
  

  #---------------------------------------------------------------------------
  #-- INSTALL TREE configuration 


  SET(CILC_BUILD_TREE_CONFIGURATION FALSE)
  # Create work directory to put the configured files because 
  # if the user invoked the macro at the root of the build tree the files 
  # would overwrite those configured for the build tree
  SET(CILC_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR}/InstallLibraryForCMake_tmp)
  IF(NOT IS_DIRECTORY ${CILC_WORK_DIR})
    FILE(MAKE_DIRECTORY ${CILC_WORK_DIR})
  ENDIF(NOT IS_DIRECTORY ${CILC_WORK_DIR})
  
  #----------------------------------------------------------------------------
  #INSTALLATION PATH
  IF(WIN32)
    SET(INSTALL_PATH .)
  ELSE(WIN32)
    IF(${LIBRARY_NAME}_INSTALL_FOLDER)
##EED      SET(INSTALL_PATH lib/${${LIBRARY_NAME}_INSTALL_FOLDER})
      SET(INSTALL_PATH ${${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS}/${${LIBRARY_NAME}_INSTALL_FOLDER})
    ELSE(${LIBRARY_NAME}_INSTALL_FOLDER)
##EED      SET(INSTALL_PATH lib/${LIBRARY_NAME})
	   SET(INSTALL_PATH ${${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS}/${LIBRARY_NAME})
    ENDIF(${LIBRARY_NAME}_INSTALL_FOLDER)
  ENDIF(WIN32)

  message(STATUS "Library Install Path ${INSTALL_PATH}")

  # UseLIBRARY_NAME.cmake 
  CONFIGURE_FILE(
    ${CREA_CMAKE_DIR}/InstallLibraryForCMake_UseLibrary.cmake.in
    ${CILC_WORK_DIR}/Use${LIBRARY_NAME}.cmake
    @ONLY IMMEDIATE
    )
  INSTALL(
    FILES 
    ${CILC_WORK_DIR}/Use${LIBRARY_NAME}.cmake
    DESTINATION ${INSTALL_PATH}
    )
  # LIBRARY_NAMEConfig.cmake 
  SET(CILC_LIB_RELATIVE_INCLUDE_PATHS 
    ${${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS})
  SET(CILC_LIB_RELATIVE_LIBRARY_PATHS 
    ${${LIBRARY_NAME}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS})
  CONFIGURE_FILE(
    ${CREA_CMAKE_DIR}/InstallLibraryForCMake_LibraryConfig.cmake.in
    ${CILC_WORK_DIR}/${LIBRARY_NAME}Config.cmake
    @ONLY IMMEDIATE
    )
  INSTALL(
    FILES 
    ${CILC_WORK_DIR}/${LIBRARY_NAME}Config.cmake
    DESTINATION ${INSTALL_PATH}
    )
  # LIBRARY_NAMEBuildSettings.cmake :
  # FCY : DEPREDCATED FUNCTION FOR CMAKE 2.8
  IF( "${CMAKE_MINIMUM_REQUIRED_VERSION}" VERSION_LESS 2.7)
  CMAKE_EXPORT_BUILD_SETTINGS(${CILC_WORK_DIR}/${LIBRARY_NAME}BuildSettings.cmake)
  ENDIF()
  INSTALL(
    FILES 
    ${CILC_WORK_DIR}/${LIBRARY_NAME}BuildSettings.cmake
    DESTINATION  ${INSTALL_PATH}
    )
  # Additional Config and Use files
  IF(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE)
    CONFIGURE_FILE(
      ${${LIBRARY_NAME}_ADDITIONAL_CONFIG_FILE}
      ${CILC_WORK_DIR}/Additional${LIBRARY_NAME}Config.cmake
      @ONLY IMMEDIATE
      )
    INSTALL(
      FILES 
      ${CILC_WORK_DIR}/Additional${LIBRARY_NAME}Config.cmake
      DESTINATION ${INSTALL_PATH}
      )
    CONFIGURE_FILE(
      ${${LIBRARY_NAME}_ADDITIONAL_USE_FILE}
      ${CILC_WORK_DIR}/AdditionalUse${LIBRARY_NAME}.cmake
      @ONLY IMMEDIATE
      )
    INSTALL(
      FILES 
      ${CILC_WORK_DIR}/AdditionalUse${LIBRARY_NAME}.cmake
      DESTINATION  ${INSTALL_PATH}
      )
  ENDIF(CILC_LIB_HAS_ADDITIONAL_CONFIG_FILE)
  # Specific to install tree
  # FindLIBRARY_NAME.cmake in CMake/Modules dir
  CONFIGURE_FILE(
    ${CREA_CMAKE_DIR}/InstallLibraryForCMake_FindLibrary.cmake.in
    ${CILC_WORK_DIR}/Find${LIBRARY_NAME}.cmake
    @ONLY IMMEDIATE
    )
#EED 02 mars 2011
#  INSTALL( 
#    FILES ${CILC_WORK_DIR}/Find${LIBRARY_NAME}.cmake
#    DESTINATION ${CMAKE_ROOT}/Modules 
#    )

# JPR 04 Mars 2011
  INSTALL( 
    FILES ${CILC_WORK_DIR}/Find${LIBRARY_NAME}.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cmake/Modules
    )
    
 
  #---------------------------------------------------------------------------
  



ENDMACRO(CREA_ADVANCED_INSTALL_LIBRARY_FOR_CMAKE)
#-----------------------------------------------------------------------------




#-----------------------------------------------------------------------------
MACRO(CREA_INSTALL_LIBRARY_FOR_CMAKE LIBRARY_NAME1)
  
  # Sets the settings to default values
  IF(NOT ${LIBRARY_NAME1}_INSTALL_FOLDER)
    SET(${LIBRARY_NAME1}_INSTALL_FOLDER ${LIBRARY_NAME1})
  ENDIF(NOT ${LIBRARY_NAME1}_INSTALL_FOLDER)

  SET(${LIBRARY_NAME1}_LIBRARIES ${LIBRARY_NAME1})
  
  FILE(RELATIVE_PATH 
    ${LIBRARY_NAME1}_BUILD_TREE_RELATIVE_INCLUDE_PATHS 
    ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    )
  IF ( ${PROJECT_BINARY_DIR} STREQUAL ${EXECUTABLE_OUTPUT_PATH} )
    SET(CILFC_EXECUTABLE_OUTPUT_REL_PATH ".")
  ELSE ( ${PROJECT_BINARY_DIR} STREQUAL ${EXECUTABLE_OUTPUT_PATH} )

    FILE(RELATIVE_PATH 
      CILFC_EXECUTABLE_OUTPUT_REL_PATH
      ${PROJECT_BINARY_DIR} ${EXECUTABLE_OUTPUT_PATH})
  ENDIF ( ${PROJECT_BINARY_DIR} STREQUAL ${EXECUTABLE_OUTPUT_PATH} )


  IF(UNIX)
    SET(${LIBRARY_NAME1}_BUILD_TREE_RELATIVE_LIBRARY_PATHS 
      ${CILFC_EXECUTABLE_OUTPUT_REL_PATH})
    SET(${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS ${CMAKE_CREA_LIB_PATH} )
  ELSE(UNIX)
    SET(${LIBRARY_NAME1}_BUILD_TREE_RELATIVE_LIBRARY_PATHS 
      ${CILFC_EXECUTABLE_OUTPUT_REL_PATH} )
# LG 15/01/09 : Bug
#      ${CILFC_EXECUTABLE_OUTPUT_REL_PATH}/Debug
#     ${CILFC_EXECUTABLE_OUTPUT_REL_PATH}/Release)
    SET(${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_LIBRARY_PATHS bin)
  ENDIF(UNIX)

  IF(NOT ${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS)
    SET(${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS include/${LIBRARY_NAME1})
  ENDIF(NOT ${LIBRARY_NAME1}_INSTALL_TREE_RELATIVE_INCLUDE_PATHS)
  
  # Invoke the advanced macro
  CREA_ADVANCED_INSTALL_LIBRARY_FOR_CMAKE(${LIBRARY_NAME1})


ENDMACRO(CREA_INSTALL_LIBRARY_FOR_CMAKE)
#-----------------------------------------------------------------------------


# This is an implementation detail for using @LIBRARY_NAME@ with the
# Find@LIBRARY_NAME@.cmake module.  Do not include directly by name.  
# This should be included only when Find@LIBRARY_NAME@.cmake sets 
# the @LIBRARY_NAME@_USE_FILE variable to point here.

IF(CREA_VERBOSE_CMAKE)
  MESSAGE(STATUS "Using @LIBRARY_NAME@")
ENDIF(CREA_VERBOSE_CMAKE)

# Load the compiler settings used for @LIBRARY_NAME@.
IF(@LIBRARY_NAME@_BUILD_SETTINGS_FILE)
  INCLUDE(CMakeImportBuildSettings)
  CMAKE_IMPORT_BUILD_SETTINGS(${@LIBRARY_NAME@_BUILD_SETTINGS_FILE})
ENDIF(@LIBRARY_NAME@_BUILD_SETTINGS_FILE)

# Add compiler flags needed to use @LIBRARY_NAME@.
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${@LIBRARY_NAME@_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${@LIBRARY_NAME@_REQUIRED_CXX_FLAGS}")
SET(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} ${@LIBRARY_NAME@_REQUIRED_LINK_FLAGS}")

# Add include directories needed to use @LIBRARY_NAME@.
INCLUDE_DIRECTORIES(${@LIBRARY_NAME@_INCLUDE_DIRS})

# Add link directories needed to use @LIBRARY_NAME@.
LINK_DIRECTORIES(${@LIBRARY_NAME@_LIBRARY_DIRS})

# Set the version 
# Already done in bbtkConfigure.h
#ADD_DEFINITIONS( -D@LIBRARY_NAME@_VERSION="\"${@LIBRARY_NAME@_VERSION}\"" )

# Additional use file 
IF (@LIBRARY_NAME@_HAS_ADDITIONAL_CONFIG_FILE)
  # Include it
  INCLUDE(${@LIBRARY_NAME@_DIR}/AdditionalUse@LIBRARY_NAME@.cmake)
ENDIF (@LIBRARY_NAME@_HAS_ADDITIONAL_CONFIG_FILE)