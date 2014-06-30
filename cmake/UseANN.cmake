MACRO(FIND_AND_USE_ANN)
  #---------------------------------------------------------------------------
  IF(NOT ANN_FOUND)

    SET(ANN_DIR "" CACHE PATH "")

    IF(ANN_DIR)

        SET(ANN_INCLUDE_DIR "${ANN_DIR}/include" CACHE PATH "" FORCE)

        #SET(ANN_LIBRARIES_DIR "${ANN_DIR}/lib" CACHE PATH "" FORCE | PARENT_SCOPE)
        LINK_DIRECTORIES(${ANN_DIR}/lib)
        SET(ANN_LIBRARIES "ANN" CACHE STRING "" FORCE)

    ENDIF(ANN_DIR)


    SET(ANN_FOUND TRUE)

  ENDIF(NOT ANN_FOUND)
  #---------------------------------------------------------------------------
ENDMACRO(FIND_AND_USE_ANN)
