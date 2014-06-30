MESSAGE(STATUS "")
MESSAGE(STATUS "=======================================")
MESSAGE(STATUS "Configuring user documentation ")

SET(ARTICLE_LINKS "<h1> Conference Articles </h1><br>")

#The name of the conference should be the same name as the folder where the *.tex file is located
SET(CONFERENCE_NAME "Sibgrapi")
#Filename of the *.tex file without extension, the *.bib file must be named the same
SET(ARTICLE_NAME "ImageSimulation_TextureSynthesis")

USER_BUILD_LATEX_DOC("1" ${CONFERENCE_NAME} ${ARTICLE_NAME})

#The name of the conference should be the same name as the folder where the *.tex file is located
SET(CONFERENCE_NAME "ISBI")
#Filename of the *.tex file without extension, the *.bib file must be named the same
SET(ARTICLE_NAME "ImageSimulation_TextureSynthesis")

USER_BUILD_LATEX_DOC("2" ${CONFERENCE_NAME} ${ARTICLE_NAME})

#The name of the conference should be the same name as the folder where the *.tex file is located
SET(CONFERENCE_NAME "SCITEPRESS") 
#Filename of the *.tex file without extension, the *.bib file must be named the same
SET(ARTICLE_NAME "3D_Texture_Synthesis_for_Modeling_Realistic_Organic_Tissues") 

USER_BUILD_LATEX_DOC("3" ${CONFERENCE_NAME} ${ARTICLE_NAME})


SET(ARTICLE_LINKS "<h1> Posters </h1><br>")
#The name of the conference should be the same name as the folder where the *.tex file is located
SET(CONFERENCE_NAME "TOPIM")
#Filename of the *.tex file without extension, the *.bib file must be named the same
SET(ARTICLE_NAME "3D_Texture_Synthesis_for_Modeling_Realistic_Organic_Tissues")


CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/UserDocumentationLinks.txt.in
    ${CMAKE_CURRENT_BINARY_DIR}/UserDocumentationLinks.txt    
    ) 



MESSAGE(STATUS "=======================================")
