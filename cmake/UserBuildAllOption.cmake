#-----------------------------------------------------------------------------
# If --BUILD_ALL is set to ON then all options 
# for which the macro SWITCH_ON_IF_BUILD_ALL have been used are set to ON
# The -- prepended to the options ensure that it will appear as the 
# first option when running ccmake/CMakeSetup
#IF (BBTK_CORE_PACKAGE)
  OPTION(--BUILD_ALL "Build all ?" OFF)
  #OPTION(--SET_BUILD_ALL_OFF "Build all ?" OFF)
  #MARK_AS_ADVANCED(--SET_BUILD_ALL_OFF)

  MACRO(SWITCH_ON_IF_BUILD_ALL VAR)
    #OPTION(--${VAR}_SET_BUILD_ALL_OFF "Build all ?" OFF)
    #MARK_AS_ADVANCED(--${VAR}_SET_BUILD_ALL_OFF)
    IF (--BUILD_ALL)
      #  IF (--${VAR}_SET_BUILD_ALL_OFF)
      #    MESSAGE(ERROR "${VAR} -> Build All OFF")
      #    SET(--BUILD_ALL OFF CACHE BOOL "Build all ?" FORCE)  
      #    SET(--${VAR}_SET_BUILD_ALL_OFF OFF CACHE BOOL "" FORCE)  
      #  ELSE (--${VAR}_SET_BUILD_ALL_OFF)
      SET(${VAR} ON CACHE BOOL "Forced to ON by --BUILD_ALL" FORCE)
      #    SET(--${VAR}_SET_BUILD_ALL_OFF ON CACHE BOOL "" FORCE)  
      #    MESSAGE(ERROR "Build All -> ${VAR}")
      #  ENDIF (--${VAR}_SET_BUILD_ALL_OFF)
    ENDIF(--BUILD_ALL)
  ENDMACRO(SWITCH_ON_IF_BUILD_ALL)
#ELSE (BBTK_CORE_PACKAGE)
 # MACRO(SWITCH_ON_IF_BUILD_ALL VAR)
 # ENDMACRO(SWITCH_ON_IF_BUILD_ALL)
#ENDIF (BBTK_CORE_PACKAGE)
#-----------------------------------------------------------------------------
