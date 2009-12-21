INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR})
SET(qcomicbook_src
        ArchiversConfiguration.cpp
        ArchiverStatus.cpp
        ArchiverStrategy.cpp
        AceArchiverStrategy.cpp
        Lens.cpp
        P7zipArchiverStrategy.cpp
        RarArchiverStrategy.cpp
        TargzArchiverStrategy.cpp
        Tarbz2ArchiverStrategy.cpp
        ZipArchiverStrategy.cpp
        FileSignature.cpp 
        AboutDialog.cpp Bookmark.cpp BookmarkManager.cpp
	Bookmarks.cpp ComicBookCfgDialog.cpp ComicBookInfo.cpp ComicBookSettings.cpp ComicMainWindow.cpp DirReader.cpp
	History.cpp IconViewThumbnail.cpp ImgArchiveSink.cpp
        ImageFormatsInfo.cpp
	ImgCache.cpp
	ImgDirSink.cpp LoaderThreadBase.cpp PageLoaderThread.cpp ImgSinkFactory.cpp
	GoToPageWidget.cpp ListViewThumbnail.cpp main.cpp Utility.cpp Page.cpp PageWidget.cpp
        PageViewBase.cpp SimplePageView.cpp ContinuousPageView.cpp
	StatusBar.cpp SystemInfoDialog.cpp
        Thumbnail.cpp ThumbnailItem.cpp ThumbnailLoaderThread.cpp
	ThumbnailsView.cpp ThumbnailsWindow.cpp
        ViewProperties.cpp
)

SET(qcomicbook_moc_hdrs
		 BookmarkManager.h 
		 ComicBookCfgDialog.h 
		 ComicBookSettings.h 
		 ComicMainWindow.h 
		 PageViewBase.h 
                 ContinuousPageView.h
                 SimplePageView.h
		 ImgDirSink.h 
		 ImgArchiveSink.h
                 LoaderThreadBase.h 
                 PageLoaderThread.h
                 ThumbnailLoaderThread.h
		 GoToPageWidget.h 
		 StatusBar.h 
		 ThumbnailsWindow.h 
		 ThumbnailsView.h
                 ViewProperties.h
)

SET(qcomicbook_ui AboutDialog.ui ConfigDialog.ui BookmarkManager.ui ComicMainWindow.ui GoToPageWidget.ui SystemInfoDialog.ui)

QT4_ADD_RESOURCES(qcomicbook_res ../data/qcomicbook.qrc)
QT4_WRAP_CPP(qcomicbook_moc_src ${qcomicbook_moc_hdrs})
QT4_WRAP_UI(qcomicbook_ui_src ${qcomicbook_ui})
ADD_EXECUTABLE(qcomicbook ${qcomicbook_src} ${qcomicbook_moc_src} ${qcomicbook_ui_src} ${qcomicbook_res})
TARGET_LINK_LIBRARIES(qcomicbook ${QT_LIBRARIES})
INSTALL(TARGETS qcomicbook DESTINATION bin)
