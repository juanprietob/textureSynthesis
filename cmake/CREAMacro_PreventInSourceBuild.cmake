# ---------------------------------------------------------------------
#
# Copyright (c) CREATIS (Centre de Recherche en Acquisition et Traitement de l'Image 
#                        pour la Santé)
# Authors : Eduardo Davila, Frederic Cervenansky, Claire Mouton
# Previous Authors : Laurent Guigues, Jean-Pierre Roux
# CreaTools website : www.creatis.insa-lyon.fr/site/fr/creatools_accueil
#
#  This software is governed by the CeCILL-B license under French law and 
#  abiding by the rules of distribution of free software. You can  use, 
#  modify and/ or redistribute the software under the terms of the CeCILL-B 
#  license as circulated by CEA, CNRS and INRIA at the following URL 
#  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html 
#  or in the file LICENSE.txt.
#
#  As a counterpart to the access to the source code and  rights to copy,
#  modify and redistribute granted by the license, users are provided only
#  with a limited warranty  and the software's author,  the holder of the
#  economic rights,  and the successive licensors  have only  limited
#  liability. 
#
#  The fact that you are presently reading this means that you have had
#  knowledge of the CeCILL-B license and that you accept its terms.
# ------------------------------------------------------------------------ */ 



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
