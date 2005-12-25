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

/*! \file comicmain.h */

#ifndef __COMIC_MAIN_H
#define __COMIC_MAIN_H

#include <kdockwidget.h>
#include "history.h"

class QAction;
class QPopupMenu;
class QLabel;
class QToolBar;
class QDockWindow;

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
	class ComicMainWindow: public KDockMainWindow
	{
		Q_OBJECT

		private:
			ImgDirSink *sink;
			ComicImageView *view;
			ThumbnailsWindow *thumbswin;
			History *recentfiles;
			Bookmarks *bookmarks;
			QComicBook::StatusBar *statusbar;
			ComicBookSettings *cfg;
			int currpage; //!<current page number
			int scrv_id; //!<identifier of "Scrollbars visible" menu option
			int contscr_id; //!<identifier of "Continous scrolling" menu option
			int firstpage_id; //!<identifier of "First page" menu option
			int lastpage_id; //!<identifier of "Last page" menu option
			int jumpto_id; //!<identifier of "Jump to" menu option
			int setbookmark_id; //!<identifier of "Set bookmark" menu option
			int rmvbookmark_id; //!<identifier of "Remove bookmark" menu option
			int close_id; //!<identifier of "Close" menu option
			int gimp_id; //<!identifier of "Open with Gimp" menu option
			int reload_id; //<!identifier of "Reload page" menu option
			QToolBar *toolbar;
			QPopupMenu *file_menu;
			QPopupMenu *settings_menu;
			QPopupMenu *edit_menu;
			QPopupMenu *context_menu;
			QPopupMenu *view_menu;
			QPopupMenu *navi_menu;
			QPopupMenu *recent_menu;
			QPopupMenu *bookmarks_menu;
			QLabel *pageinfo; //!<page info displayed in right-click context menu
			QString lastdir; //!<last opened directory for Open File/Directory dialog
			QAction *toggleThumbnailsAction;
			QAction *toggleToolbarAction;
			QAction *toggleStatusbarAction;
			QAction *showInfoAction;
			QAction *nextPageAction;
			QAction *prevPageAction;
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

		protected:
			void keyPressEvent(QKeyEvent *e);
			void closeEvent(QCloseEvent *e);
			bool confirmExit();
			void enableComicBookActions(bool f=true);
			void saveSettings();

			void setupActions();
			void setupComicImageView();
			void setupThumbnailsWindow();
			void setupToolbar();
			void setupFileMenu();
			void setupSettingsMenu();
			void setupEditMenu();
			void setupViewMenu();
			void setupNavigationMenu();
			void setupBookmarksMenu();
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
			void thumbnailsVisibilityChanged(bool f);
			void toolbarVisibilityChanged(bool f);
			void openWithGimp();

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
			void showInfo();
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
