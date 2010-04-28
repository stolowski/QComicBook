QT4_ADD_TRANSLATION(qcomicbook_qm
                        qcomicbook_pl_PL.ts
                   )

ADD_CUSTOM_TARGET(translations DEPENDS ${qcomicbook_qm})
INSTALL(FILES ${qcomicbook_qm} DESTINATION share/${PACKAGE}/i18n)
