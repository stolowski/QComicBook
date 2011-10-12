INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/FrameDetect
        ${CMAKE_SOURCE_DIR}/src/Job
	${CMAKE_BINARY_DIR}/src
	${CMAKE_BINARY_DIR}/src/FrameDetect
        ${CMAKE_BINARY_DIR}/src/Job
	${CMAKE_BINARY_DIR}
	${POPPLER_INCLUDE_DIRS}
)

SET(qcomicbook_moc_hdrs
	FrameDetect/FrameCache.h
	FrameDetect/FrameDetectThread.h
	Archivers/ArchiversConfiguration.h
	BookmarkManager.h 
	ComicBookCfgDialog.h 
	ComicBookSettings.h 
	ComicMainWindow.h 
	View/FrameView.h
	View/PageViewBase.h 
	View/ContinuousPageView.h
	View/SimplePageView.h
	Sink/ImgSink.h
        Sink/ImgDirSink.h 
	Sink/ImgArchiveSink.h
	LoaderThreadBase.h 
	PageLoaderThread.h
	ThumbnailLoaderThread.h
	GoToPageWidget.h 
        PrinterThread.h
        PrintProgressDialog.h
        RecentFilesMenu.h
	StatusBar.h 
        SystemInfoDialog.h
	ThumbnailsWindow.h 
	ThumbnailsView.h
	ViewProperties.h
        Job/ImageTransformThread.h
)

file (GLOB_RECURSE qcomicbook_hdr *.h *.hpp)
file (GLOB_RECURSE qcomicbook_src *.cpp)
file (GLOB qcomicbook_ui *.ui)

SET(FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${qcomicbook_src}
	${qcomicbook_ui} ${qcomicbook_hdr} PARENT_SCOPE)

QT4_ADD_RESOURCES(qcomicbook_res ../data/qcomicbook.qrc)
QT4_WRAP_CPP(qcomicbook_moc_src ${qcomicbook_moc_hdrs})
QT4_WRAP_UI(qcomicbook_ui_src ${qcomicbook_ui})

ADD_EXECUTABLE(qcomicbook ${qcomicbook_src} ${qcomicbook_moc_src} ${qcomicbook_ui_src} ${qcomicbook_res})
ADD_DEPENDENCIES(qcomicbook translations)
TARGET_LINK_LIBRARIES(qcomicbook ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(qcomicbook ${POPPLER_LIBRARIES})

INSTALL(TARGETS qcomicbook DESTINATION bin)

