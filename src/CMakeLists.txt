INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
SET(qcomicbook_src AboutDialog.cpp Bookmark.cpp BookmarkManager.cpp
	Bookmarks.cpp ComicBookCfgDialog.cpp ComicBookInfo.cpp ComicBookSettings.cpp ComicMainWindow.cpp DirReader.cpp
	History.cpp Icons.cpp IconViewThumbnail.cpp ImgArchiver.cpp ImgArchiveSink.cpp
	ImgCache.cpp
	ImgDirSink.cpp ImgLoader.cpp ImgSinkFactory.cpp ComicImageView.cpp
	JumpToPageWindow.cpp ListViewThumbnail.cpp main.cpp Utility.cpp Page.cpp PageNumberEdit.cpp
	StatusBar.cpp SupportedArchivesWindow.cpp Thumbnail.cpp
	ThumbnailEvent.cpp ThumbnailItem.cpp ThumbnailLoader.cpp
	ThumbnailsView.cpp ThumbnailsWindow.cpp)

SET(qcomicbook_moc_hdrs
		 BookmarkManager.h 
		 ComicBookCfgDialog.h 
		 ComicBookSettings.h 
		 ComicMainWindow.h 
		 ImgArchiver.h 
		 ComicImageView.h 
		 ImgDirSink.h 
		 ImgArchiveSink.h 
		 JumpToPageWindow.h 
		 PageNumberEdit.h 
		 StatusBar.h 
		 SupportedArchivesWindow.h 
		 ThumbnailsWindow.h 
		 ThumbnailsView.h)

QT4_WRAP_CPP(qcomicbook_moc_src ${qcomicbook_moc_hdrs})
ADD_EXECUTABLE(qcomicbook ${qcomicbook_src} ${qcomicbook_moc_src})
TARGET_LINK_LIBRARIES(qcomicbook ${QT_LIBRARIES})
