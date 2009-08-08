bin_PROGRAMS = qcomicbook

BUILT_SOURCES = \
		moc_BookmarkManager.cpp \
		moc_ComicBookCfgDialog.cpp \
		moc_ComicBookSettings.cpp \
		moc_ComicMainWindow.cpp \
		moc_ImgArchiver.cpp \
		moc_ImgArchiveSink.cpp \
		moc_ImgDirSink.cpp \
		moc_ComicImageView.cpp \
		moc_JumpToPageWindow.cpp \
		moc_PageNumberEdit.cpp \
		moc_StatusBar.cpp \
		moc_SupportedArchivesWindow.cpp \
		moc_ThumbnailsWindow.cpp \
		moc_ThumbnailsView.cpp

qcomicbook_SOURCES = AboutDialog.cpp Bookmark.cpp BookmarkManager.cpp \
	Bookmarks.cpp ComicBookCfgDialog.cpp ComicBookInfo.cpp ComicBookSettings.cpp ComicMainWindow.cpp DirReader.cpp \
	History.cpp Icons.cpp IconViewThumbnail.cpp ImgArchiver.cpp ImgArchiveSink.cpp \
	ImgCache.cpp \
	ImgDirSink.cpp ImgLoader.cpp ImgSinkFactory.cpp ComicImageView.cpp \
	JumpToPageWindow.cpp ListViewThumbnail.cpp main.cpp Utility.cpp PageNumberEdit.cpp \
	StatusBar.cpp SupportedArchivesWindow.cpp Thumbnail.cpp \
	ThumbnailEvent.cpp ThumbnailItem.cpp ThumbnailLoader.cpp \
	ThumbnailsView.cpp ThumbnailsWindow.cpp

#dist_qcomicbook_SOURCES = $(qcomicbook_SOURCES)
		      
nodist_qcomicbook_SOURCES = $(BUILT_SOURCES)

noinst_HEADERS = AboutDialog.h \
		 Bookmark.h \
		 BookmarkManager.h \
		 Bookmarks.h \
		 ComicBookCfgDialog.h \
		 cbicons.h \
		 ComicBookInfo.h \
		 ComicBookSettings.h \
		 ComicMainWindow.h \
		 DirReader.h \
		 EnumMap.h \
		 History.h \
		 Icons.h \
		 IconViewThumbnail.h \
		 ImgArchiver.h \
		 ImgCache.h \
		 ComicImageView.h \
		 ImgSinkFactory.h \
		 ImgDirSink.h \
		 ImgArchiveSink.h \
		 ImgLoader.h \
		 JumpToPageWindow.h \
		 ListViewThumbnail.h \
		 Utility.h \
		 PageNumberEdit.h \
		 StatusBar.h \
		 SupportedArchivesWindow.h \
		 Thumbnail.h \
		 ThumbnailEvent.h \
		 ThumbnailItem.h \
		 ThumbnailLoader.h \
		 ThumbnailsWindow.h \
		 ThumbnailsView.h
		 
MOSTLYCLEANFILES =+ $(BUILT_SOURCES)

qcomicbook_LDFLAGS = $(QtGui_LIBS)
qcomicbook_CPPFLAGS = -DDATADIR=\"$(datadir)/qcomicbook/\" -DQT_THREAD_SUPPORT $(QtGui_CFLAGS)
qcomicbook_CXXFLAGS = $(qcomicbook_CPPFLAGS)

moc_%.cpp: %.h
	moc-qt4 $< -o $@


