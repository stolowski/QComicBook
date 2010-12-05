file (GLOB translation_src *.ts)

QT4_CREATE_TRANSLATION(qcomicbook_qm ${FILES_TO_TRANSLATE} ${translation_src})
SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY CLEAN_NO_CUSTOM ON)
ADD_CUSTOM_TARGET(translations DEPENDS ${qcomicbook_qm})
INSTALL(FILES ${qcomicbook_qm} DESTINATION share/${PACKAGE}/i18n)
