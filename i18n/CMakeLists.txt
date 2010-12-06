QT4_ADD_TRANSLATION(qcomicbook_qm
			qcomicbook_de_DE.ts
                        qcomicbook_es_ES.ts
                        qcomicbook_it_IT.ts
                        qcomicbook_fi_FI.ts
                        qcomicbook_fr_FR.ts
                        qcomicbook_fr_CA.ts
			qcomicbook_cs_CZ.ts
                        qcomicbook_ko_KR.ts
                        qcomicbook_nl_NL.ts
                        qcomicbook_pl_PL.ts
                        qcomicbook_pt_BR.ts
			qcomicbook_ru_RU.ts
                        qcomicbook_uk_UA.ts
			qcomicbook_zh_CN.ts
                   )

ADD_CUSTOM_TARGET(translations DEPENDS ${qcomicbook_qm})
INSTALL(FILES ${qcomicbook_qm} DESTINATION share/${PACKAGE}/i18n)
