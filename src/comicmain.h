/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

/*! \file comicmain.h */

#ifndef __COMIC_MAIN_H
#define __COMIC_MAIN_H

#include <QMainWindow>
#include "history.h"

class QAction;
class QPopupMenu;
class QLabel;
class QToolBar;
class QDockWindow;
class QKeyEvent;

namespace QComicBook
{
	class ImgDirSink;
	class ComicBookSettings;
	class ComicImageView;
	class ThumbnailsWindow;
	class Bookmarks;
	class StatusBar;
	using Utility::History;

	//! The main window of QComicBook.
	class ComicMainWindow: public QMainWindow
	{
		Q_OBJECT

		private:
			ImgDirSink *sink;
			ComicImageView *view;
			ThumbnailsWindow *thumbswin;
			History *recentfiles;
			Bookmarks *bookmarks;
			StatusBar *statusbar;
			ComicBookSettings *cfg;
			int currpage; //!<current page number
					
			QToolBar *toolbar;
			QMenu *file_menu;
			QMenu *edit_menu;
			QMenu *context_menu;
			QMenu *view_menu;
			QMenu *navi_menu;
			QMenu *recent_menu;
			QMenu *bookmarks_menu;
			QMenu *settings_menu;
			QLabel *pageinfo; //!<page info displayed in right-click context menu
			QString lastdir; //!<last opened directory for Open File/Directory dialog
			QAction *toggleScrollbarsAction;
			QAction *setBookmarkAction;
			QAction *removeBookmarkAction;
			QAction *closeAction;
			QAction *gimpAction;
			QAction *reloadAction;
			QAction *jumpToAction;
			QAction *contScrollAction;
			QAction *toggleThumbnailsAction;
			QAction *toggleStatusbarAction;
			QAction *showInfoAction;
			QAction *nextPageAction;
			QAction *prevPageAction;
			QAction *firstPageAction;
			QAction *lastPageAction;
			QAction *fullScreenAction;
			QAction *exitFullScreenAction;
			QAction *bestFitAction;
			QAction *fitWidthAction;
			QAction *fitHeightAction;
			QAction *wholePageAction;
			QAction *originalSizeAction;
			QAction *forwardPageAction;
			QAction *backwardPageAction;
			QAction *pageTopAction;
			QAction *pageBottomAction;
			QAction *mangaModeAction;
			QAction *twoPagesAction;
			QAction *openNextAction;
			QAction *openPrevAction;
			QAction *rotateRightAction;
			QAction *rotateLeftAction;
			QAction *rotateResetAction;
			QAction *scrollLeftFastAction;
			QAction *scrollRightFastAction;
			QAction *scrollUpFastAction; 
			QAction *scrollDownFastAction;
			QAction *scrollLeftAction;
			QAction *scrollRightAction;   
			QAction *scrollUpAction;
			QAction *scrollDownAction;
			QAction *jumpDownAction;
			QAction *jumpUpAction;
			QAction *togglePreserveRotationAction;
			QAction *openArchiveAction;
			QAction *openDirAction;
			QAction *quitAction;
			QAction *savePageAction;

		protected:
			virtual void dragEnterEvent(QDragEnterEvent *e);
			virtual void dropEvent(QDropEvent *e);
			virtual void keyPressEvent(QKeyEvent *e);
			virtual void closeEvent(QCloseEvent *e);

			bool confirmExit();
			void enableComicBookActions(bool f=true);
			void saveSettings();

			void setupActions();
			void setupComicImageView();
			void setupThumbnailsWindow();
			void setupToolbar();
			void setupFileMenu();
			void setupEditMenu();
			void setupViewMenu();
			void setupNavigationMenu();
			void setupBookmarksMenu();
			void setupSettingsMenu();
			void setupHelpMenu();
			void setupStatusbar();
			void setupContextMenu();

		protected slots:
			void sinkReady(const QString &path);
			void sinkError(int code);
			void updateCaption();
			void setRecentFilesMenu(const History &hist);
			void recentSelected(int id);
			void bookmarkSelected(int id);
			void thumbnailsWindowShown();
			void openWithGimp();
			void savePageAs();

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
			void createArchive();
			void open(const QString &path, int page=0);
			void openNext();
			void openPrevious();
			void showAbout();
			void showHelp();
			void showInfo();
			void showSysInfo();
			void showConfigDialog();
			void showJumpToPage(const QString &number=QString::null);
			void closeSink();
			void setBookmark();
			void removeBookmark();
			void openBookmarksManager();
			void toggleScrollbars();
			void toggleTwoPages(bool f);
			void toggleFullScreen();
			void toggleContinousScroll();
			void toggleJapaneseMode(bool f);
			void reloadPage();

		public:
			ComicMainWindow(QWidget *parent);
			virtual ~ComicMainWindow();
	};
}

#endif
