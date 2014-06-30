#-----------------------------------------------------------------------------
# CPACK+NSIS PACKAGING
#-----------------------------------------------------------------------------

#---Documentation-------------
#http://www.cmake.org/Wiki/CMake:Packaging_With_CPack
#http://www.cmake.org/Wiki/CMake:CPackConfiguration
#http://www.cmake.org/Wiki/CMake:Install_Commands


INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - CREATIS-LRMN")
SET(CPACK_PACKAGE_VENDOR "CREATIS")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_MINOR_VERSION})
SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_BUILD_VERSION})


#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CreaTools\\\\${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")



IF(WIN32)
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CreaTools\\\\${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
ELSE(WIN32)
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CreaTools/${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
ENDIF(WIN32)
  


IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
#EED  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/kernel/install/gnome/bbi-icon2.bmp")
#  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin/BBI_EXE.exe")
  SET(CPACK_NSIS_DISPLAY_NAME ${PROJECT_NAME})
  SET(CPACK_NSIS_HELP_LINK "http://www.creatis.insa-lyon.fr/site/creatools/${PROJECT_NAME}")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.creatis.insa-lyon.fr/site/creatools/${PROJECT_NAME}")
  SET(CPACK_NSIS_CONTACT "info-dev@creatis.insa-lyon.fr")
  SET(CPACK_NSIS_MODIFY_PATH ON)
  
     SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS  "
	  # NSIS code ..	
   ")
		
	SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS  "
	  # NSIS code ...
	")
   

  
ELSE(WIN32 AND NOT UNIX)
#  SET(CPACK_STRIP_FILES "bin/bbi")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
#SET(CPACK_PACKAGE_EXECUTABLES "TestWxGimmickDialog" "Gimmick!")
  

INCLUDE(CPack)




IF(EXWIN32)

OPTION(STANDALONE_INSTALLER_${PROJECT_NAME} "Install and add to installer all dlls needed, including from external toolkits (wx,itk,vtk)?" OFF)

IF(BBTK_STANDALONE_INSTALLER)
  STRING(REGEX REPLACE "\\\\" "/" WINDIR "$ENV{WINDIR}")
#  INSTALL(   FILES ${WINDIR}/system32/msvcp71d.dll  DESTINATION bin/ )
#  INSTALL(   FILES ${WINDIR}/system32/msvcr71d.dll  DESTINATION bin/ )

  IF(USE_VTK)
    SET(VTK_BIN_DIR ${VTK_DIR}/../../bin )

    FILE(GLOB LST_FILE_VTKDLL "${VTK_BIN_DIR}/*.dll")
    FOREACH( iLST  ${LST_FILE_VTKDLL} )
       INSTALL( 
         FILES ${iLST}
         DESTINATION bin/
       )
    ENDFOREACH(iLST)
  ENDIF(USE_VTK)
    
  IF(USE_ITK)
    INSTALL(   FILES ${ITK_DIR}/bin/ITKCommon.dll   DESTINATION bin/ )
  ENDIF(USE_ITK)

  IF(USE_WXWIDGETS)
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_xrc_vc_custom.dll        DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_richtext_vc_custom.dll   DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_qa_vc_custom.dll         DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_media_vc_custom.dll      DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_html_vc_custom.dll       DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_gl_vc_custom.dll         DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_core_vc_custom.dll       DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_aui_vc_custom.dll        DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxmsw28d_adv_vc_custom.dll        DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxbase28d_xml_vc_custom.dll       DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxbase28d_vc_custom.dll           DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxbase28d_odbc_vc_custom.dll      DESTINATION bin/ )
    INSTALL(   FILES ${WXWIDGETS_ROOT_DIR}/lib/vc_dll/wxbase28d_net_vc_custom.dll       DESTINATION bin/ )
  ENDIF(USE_WXWIDGETS)
ENDIF(BBTK_STANDALONE_INSTALLER)

## Use ...


ENDIF(EXWIN32)
