/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "PageViewBase.h"
#include "EnumMap.h"
#include <QObject>
#include <QFont>
#include <QByteArray>

class QSettings;
class QColor;

namespace QComicBook
{
	using namespace Utility;

	class ComicMainWindow;
        
	class ComicBookSettings: public QObject
	{
		Q_OBJECT

		signals:
			void displaySettingsChanged(const QString &option);

		public:
			void load();

			bool embedPageNumbers() const;
			bool smallCursor() const;
			bool twoPagesMode() const;
			bool japaneseMode() const;
			bool continuousScrolling() const;
			ViewType viewType() const;
			bool scrollbarsVisible() const;
			Size pageSize() const;
			bool smoothScaling() const;
			QString lastDir() const;
			const QStringList& recentlyOpened() const;
			QColor background() const;
			int cacheSize() const;
			bool cacheAutoAdjust() const;
			bool cacheThumbnails() const;
			int thumbnailsAge() const;
			bool preloadPages() const;
			bool confirmExit() const;
			bool autoInfo() const;
			bool fullScreenHideMenu() const;
			bool fullScreenHideStatusbar() const;
			bool fullScreenHideToolbar() const;
			bool showStatusbar() const;
			const QFont& infoFont() const;
			void restoreGeometry(ComicMainWindow *w) const;
			void restoreDockLayout(ComicMainWindow *w) const;
			bool showSplash() const;
			QString tmpDir() const;
			bool showDonationDialog() const;

			void embedPageNumbers(bool f);
			void smallCursor(bool f);
			void twoPagesMode(bool f);
			void japaneseMode(bool f);
			void continuousScrolling(bool f);
			void viewType(ViewType t);
			void scrollbarsVisible(bool f);
			void pageSize(Size s);
			void smoothScaling(bool s);
			void lastDir(const QString &d);
			void recentlyOpened(const QStringList &hist);
			void background(const QColor &color);
			void cacheSize(int s);
			void cacheAutoAdjust(bool f);
			void cacheThumbnails(bool f);
			void thumbnailsAge(int n);
			void preloadPages(bool f);
			void confirmExit(bool f);
			void autoInfo(bool f);
			void fullScreenHideMenu(bool f);
			void fullScreenHideStatusbar(bool f);
			void fullScreenHideToolbar(bool f);
			void showStatusbar(bool f);
			void infoFont(const QFont &f);
			void saveDockLayout(ComicMainWindow *w);
			void saveGeometry(ComicMainWindow *w);
			void showSplash(bool f);
			void tmpDir(const QString &dir);
			bool showDonationDialog(bool f);

			static ComicBookSettings& instance();

			//
			// checks and creates .qcomicbook/ and cache subdirectories if
			// necessary
			bool checkDirs();
			const QString& bookmarksDir();
			const QString& thumbnailsDir();

		private:
			QSettings *m_cfg;
			bool m_embedpagenumbers;
			bool m_smallcursor;
			bool m_twopages;
			bool m_twopagesstep;
			bool m_japanese;
			bool m_contscroll;
			bool m_scrollbars;
			bool m_preload;
			bool m_fscrhidemenu;
			bool m_fscrhidestatus;
			bool m_fscrhidetoolbar;
			Size m_pagesize;
			ViewType m_viewtype;
			bool m_smoothscaling;
			QString m_lastdir;
			QColor m_bgcolor;
			QStringList m_recent;
			int m_cachesize;
			int m_thumbsage;
			bool m_cacheadjust;
			bool m_cachethumbs;
			bool m_autoinfo;
			bool m_confirmexit;
			bool m_statusbar;
			bool m_intbrowser;
			bool m_showsplash;
			bool m_donationdlg;
			QString m_tmpdir;
			QFont m_font;

			QString m_bkpath; //bookmarks path
			QString m_thpath; //thumbnails cache path
			bool m_dirsok; //is above dirs are ok

			static const EnumMap<Size> size2string[];
			static const EnumMap<ViewType> viewtype2string[];

			ComicBookSettings();
			ComicBookSettings(const ComicBookSettings &);
			ComicBookSettings operator =(const ComicBookSettings &);
			virtual ~ComicBookSettings();


	};
}

#endif

