bin_PROGRAMS = qcomicbook

BUILT_SOURCES = moc_bookmarkmanager.cpp \
		moc_cbconfigdialog.cpp \
		moc_cbsettings.cpp \
		moc_comicmain.cpp \
		moc_helpbrowser.cpp \
		moc_imgarchivesink.cpp \
		moc_imgdirsink.cpp \
		moc_imgsink.cpp \
		moc_imgview.cpp \
		moc_jumptopagewin.cpp \
		moc_pagenumberedit.cpp \
		moc_statusbar.cpp \
		moc_thumbnailswin.cpp \
		moc_thumbnailsview.cpp \
		moc_waitdialog.cpp 

dist_qcomicbook_SOURCES = aboutdialog.cpp \
		      bookmarks.cpp \
		      bookmarkmanager.cpp \
		      cbconfigdialog.cpp \
		      cbinfo.cpp \
		      cbsettings.cpp \
		      helpbrowser.cpp \
		      history.cpp \
		      icons.cpp \
		      imgcache.cpp \
		      comicmain.cpp \
		      imgloader.cpp \
		      imgsink.cpp \
		      imgdirsink.cpp \
		      imgarchivesink.cpp \
		      imgview.cpp \
		      jumptopagewin.cpp \
		      main.cpp \
		      miscutil.cpp \
		      pagenumberedit.cpp \
		      statusbar.cpp \
		      thumbnail.cpp \
		      thumbnailitem.cpp \
		      thumbnailloader.cpp \
		      thumbnailswin.cpp \
		      thumbnailsview.cpp \
		      waitdialog.cpp
		      
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
		 helpbrowser.h \
		 history.h \
		 icons.h \
		 imgcache.h \
		 imgview.h \
		 imgsink.h \
		 imgdirsink.h \
		 imgarchivesink.h \
		 imgloader.h \
		 jumptopagewin.h \
		 miscutil.h \
		 pagenumberedit.h \
		 statusbar.h \
		 thumbnail.h \
		 thumbnailitem.h \
		 thumbnailloader.h \
		 thumbnailswin.h \
		 thumbnailsview.h \
		 waitdialog.h
		 
MOSTLYCLEANFILES =+ $(BUILT_SOURCES)

qcomicbook_LDFLAGS = $(QT_LIBS)
qcomicbook_CPPFLAGS = -DDATADIR=\"$(datadir)/qcomicbook/\" -DQT_THREAD_SUPPORT $(QT_CXXFLAGS)
qcomicbook_CXXFLAGS = $(qcomic_CPPFLAGS)

moc_%.cpp: %.h
	$(QT_MOC) $< -o $@

