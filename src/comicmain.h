/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __COMIC_MAIN_H
#define __COMIC_MAIN_H

#include <qmainwindow.h>

class QAction;
class QPopupMenu;
class QLabel;
class QToolBar;
class QDockWindow;
class ImgSink;
class ComicImageView;
class ThumbnailsWindow;
class ComicBookSettings;
class History;
class Bookmarks;
class StatusBar;

class ComicMainWindow: public QMainWindow
{
	Q_OBJECT
		
	private:
		ImgSink *sink;
		ComicImageView *view;
		ThumbnailsWindow *thumbswin;
		ComicBookSettings *cfg;
		int currpage;
		int scrv_id;
		int contscr_id;
		int opennext_id;
		int openprv_id;
		int firstpage_id;
		int lastpage_id;
		int jumpto_id;
		int setbookmark_id;
		int rmvbookmark_id;
		int close_id;
		QToolBar *toolbar;
		QPopupMenu *file_menu;
		QPopupMenu *context_menu;
		QPopupMenu *view_menu;
		QPopupMenu *navi_menu;
		QPopupMenu *recent_menu;
		QPopupMenu *bookmarks_menu;
		QLabel *pageinfo;
		QString lastdir;
		History *recentfiles;
		Bookmarks *bookmarks;
		StatusBar *statusbar;
		QAction *toggleThumbnailsAction;
		QAction *toggleToolbarAction;
		QAction *toggleStatusbarAction;
		QAction *showInfoAction;
		QAction *nextPageAction;
		QAction *prevPageAction;
		QAction *forwardPageAction;
		QAction *backwardPageAction;
		QAction *pageTopAction;
		QAction *pageBottomAction;
		QAction *mangaModeAction;
		QAction *twoPagesAction;
		static const QString ARCH_EXTENSIONS;
		
	protected:
		void keyPressEvent(QKeyEvent *e);
		void closeEvent(QCloseEvent *e);
		bool confirmExit();
		void enableComicBookActions(bool f=true);
		
	protected slots:
		void sinkReady(const QString &path);
		void sinkError(int code);
		void updateCaption();
		void setRecentFilesMenu(const History &hist);
		void recentSelected(int id);
		void bookmarkSelected(int id);
		void toggleScrollbars();
		void toggleTwoPages(bool f);
		void toggleFullScreen();
		void toggleContinousScroll();
		void toggleJapaneseMode(bool f);
		void thumbnailsVisibilityChanged(bool f);
		void toolbarVisibilityChanged(bool f);

	public slots:
		void firstPage();
		void lastPage();
		void nextPage();
		void prevPage();
		void prevPageBottom();
		void forwardPages();
		void backwardPages();
		void jumpToPage(int n, bool force=false);
		void exitFullscreen();
		void browseDirectory();
		void browseArchive();
		void open(const QString &path);
		void openDir(const QString &name);
		void openArchive(const QString &name);
		void openNext();
		void openPrevious();
		void showAbout();
		void showHelp();
		void showInfo();
		void showConfigDialog();
		void showJumpToPage(const QString &number=QString::null);
		void closeSink();
		void setBookmark();
		void removeBookmark();

	public:
		ComicMainWindow(QWidget *parent);
		virtual ~ComicMainWindow();
};

#endif
