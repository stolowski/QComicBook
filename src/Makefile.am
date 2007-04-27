bin_PROGRAMS = qcomicbook

BUILT_SOURCES = \
		moc_bookmarkmanager.cpp \
		moc_cbconfigdialog.cpp \
		moc_cbsettings.cpp \
		moc_comicmain.cpp \
		moc_helpbrowser.cpp \
		moc_imgarchiver.cpp \
		moc_imgarchivesink.cpp \
		moc_imgdirsink.cpp \
		moc_imgview.cpp \
		moc_jumptopagewin.cpp \
		moc_pagenumberedit.cpp \
		moc_statusbar.cpp \
		moc_suparchwin.cpp \
		moc_thumbnailswin.cpp \
		moc_thumbnailsview.cpp

qcomicbook_SOURCES = aboutdialog.cpp bookmark.cpp bookmarkmanager.cpp\
	bookmarks.cpp cbconfigdialog.cpp cbinfo.cpp cbsettings.cpp comicmain.cpp\
	helpbrowser.cpp history.cpp icons.cpp iconviewthumbnail.cpp imgarchiver.cpp imgarchivesink.cpp \
	imgcache.cpp \
	imgdirsink.cpp imgloader.cpp imgsinkfactory.cpp imgview.cpp\
	jumptopagewin.cpp listviewthumbnail.cpp main.cpp miscutil.cpp pagenumberedit.cpp\
	statusbar.cpp suparchwin.cpp thumbnail.cpp\
	thumbnailevent.cpp thumbnailitem.cpp thumbnailloader.cpp\
	thumbnailsview.cpp thumbnailswin.cpp

#dist_qcomicbook_SOURCES = $(qcomicbook_SOURCES)
		      
nodist_qcomicbook_SOURCES = $(BUILT_SOURCES)

noinst_HEADERS = aboutdialog.h \
		 bookmark.h \
		 bookmarkmanager.h \
		 bookmarks.h \
		 cbconfigdialog.h \
		 cbicons.h \
		 cbinfo.h \
		 cbsettings.h \
		 comicmain.h \
		 enummap.h \
		 helpbrowser.h \
		 history.h \
		 icons.h \
		 iconviewthumbnail.h \
		 imgarchiver.h \
		 imgcache.h \
		 imgview.h \
		 imgsinkfactory.h \
		 imgdirsink.h \
		 imgarchivesink.h \
		 imgloader.h \
		 jumptopagewin.h \
		 listviewthumbnail.h \
		 miscutil.h \
		 pagenumberedit.h \
		 statusbar.h \
		 suparchwin.h \
		 thumbnail.h \
		 thumbnailevent.h \
		 thumbnailitem.h \
		 thumbnailloader.h \
		 thumbnailswin.h \
		 thumbnailsview.h
		 
MOSTLYCLEANFILES =+ $(BUILT_SOURCES)

qcomicbook_LDFLAGS = $(QtGui_LIBS)
qcomicbook_CPPFLAGS = -DDATADIR=\"$(datadir)/qcomicbook/\" -DQT_THREAD_SUPPORT $(QtGui_CFLAGS)
qcomicbook_CXXFLAGS = $(qcomicbook_CPPFLAGS)

moc_%.cpp: %.h
	moc-qt4 $< -o $@


