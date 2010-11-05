INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/FrameDetect
	${CMAKE_BINARY_DIR}/src
	${CMAKE_BINARY_DIR}/src/FrameDetect
	${CMAKE_BINARY_DIR}
	${POPPLER_INCLUDE_DIRS}
)

SET(qcomicbook_src
	FrameDetect/BinarizedImage.cpp
	FrameDetect/ComicFrameList.cpp
	FrameDetect/FrameDetect.cpp
	FrameDetect/FrameDetectThread.cpp
	FrameDetect/FrameCache.cpp
	Archivers/ArchiversConfiguration.cpp
        Archivers/ArchiverHint.cpp
	Archivers/ArchiverStatus.cpp
	Archivers/ArchiverStrategy.cpp
	Archivers/AceArchiverStrategy.cpp
	Archivers/P7zipArchiverStrategy.cpp
	Archivers/RarArchiverStrategy.cpp
	Archivers/TargzArchiverStrategy.cpp
	Archivers/Tarbz2ArchiverStrategy.cpp
	Archivers/ZipArchiverStrategy.cpp
	FileSignature.cpp 
	AboutDialog.cpp
	Bookmark.cpp
	BookmarkManager.cpp
	Bookmarks.cpp
	ComicBookCfgDialog.cpp
	ComicBookInfo.cpp
	ComicBookSettings.cpp
	ComicImageWidget.cpp
	ComicMainWindow.cpp
	DirReader.cpp
	FrameView.cpp
	FrameWidget.cpp
	History.cpp
	IconViewThumbnail.cpp
	ImgArchiveSink.cpp
	ImageFormatsInfo.cpp
	ImgCache.cpp
        ImgPdfSink.cpp
	ImgDirSink.cpp
	LoaderThreadBase.cpp
	PageLoaderThread.cpp
        PrintProgressDialog.cpp
	ImgSinkFactory.cpp
	GoToPageWidget.cpp
	ListViewThumbnail.cpp
	main.cpp
	Utility.cpp
	Page.cpp
	PageWidget.cpp
	PageViewBase.cpp
        PrinterThread.cpp
	SimplePageView.cpp
	ContinuousPageView.cpp
        RecentFilesMenu.cpp
	StatusBar.cpp
	SystemInfoDialog.cpp
	Thumbnail.cpp
	ThumbnailItem.cpp
	ThumbnailLoaderThread.cpp
	ThumbnailsView.cpp
	ThumbnailsWindow.cpp
	ViewProperties.cpp
)

SET(qcomicbook_moc_hdrs
	FrameDetect/FrameCache.h
	FrameDetect/FrameDetectThread.h
	Archivers/ArchiversConfiguration.h
	BookmarkManager.h 
	ComicBookCfgDialog.h 
	ComicBookSettings.h 
	ComicImageWidget.h
	ComicMainWindow.h 
	FrameView.h
	PageViewBase.h 
	ContinuousPageView.h
	SimplePageView.h
	ImgSink.h 
	ImgArchiveSink.h
	LoaderThreadBase.h 
	PageLoaderThread.h
	ThumbnailLoaderThread.h
	GoToPageWidget.h 
	PageWidget.h
        PrinterThread.h
        PrintProgressDialog.h
        RecentFilesMenu.h
	StatusBar.h 
        SystemInfoDialog.h
	ThumbnailsWindow.h 
	ThumbnailsView.h
	ViewProperties.h
)

SET(qcomicbook_ui
	AboutDialog.ui
	ConfigDialog.ui
	DonationDialog.ui
	BookmarkManager.ui
	ComicMainWindow.ui
	GoToPageWidget.ui
        PrintProgressDialog.ui
	SystemInfoDialog.ui
)

QT4_ADD_RESOURCES(qcomicbook_res ../data/qcomicbook.qrc)
QT4_WRAP_CPP(qcomicbook_moc_src ${qcomicbook_moc_hdrs})
QT4_WRAP_UI(qcomicbook_ui_src ${qcomicbook_ui})
###QT4_ADD_TRANSLATION(qcomicbook_qm ../lang/pl_PL.ts)

ADD_EXECUTABLE(qcomicbook ${qcomicbook_src} ${qcomicbook_moc_src} ${qcomicbook_ui_src} ${qcomicbook_res})
ADD_DEPENDENCIES(qcomicbook translations)
TARGET_LINK_LIBRARIES(qcomicbook ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(qcomicbook ${POPPLER_LIBRARIES})

INSTALL(TARGETS qcomicbook DESTINATION bin)

