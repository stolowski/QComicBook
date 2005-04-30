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

#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "imgview.h"
#include "history.h"
#include <qobject.h>
#include <qfont.h>

class QSettings;
class QRect;
class QColor;
class QMainWindow;

class ComicBookSettings: public QObject
{
	Q_OBJECT
		
	private:
		QSettings *cfg;
		bool smallcursor;
		bool twopages;
		bool twopagesstep;
		bool japanese;
		bool contscroll;
		bool scrollbars;
		bool preload;
		bool fscrhidemenu;
		bool fscrhidestatus;
		ComicImageView::Size pagesize;
		ComicImageView::Scaling scaling;
		int x, y, w, h;
		QString lastdir;
		QColor bgcolor;
		History recent;
		int cachesize;
		int thumbsage;
		bool cachethumbs;
		bool autoinfo;
		bool confirmexit;
		bool statusbar;
		QString docklayout;
		QFont font;
		
		static QString bkpath; //bookmarks path
		static QString thpath; //thumbnails cache path
		static bool dirsok; //is above dirs are ok

		struct int2qstring { //struct for mapping enum to strings
			int v;
			QString str;
		};

		static const int2qstring size2string[];
		static const int2qstring scaling2string[];

		const QString& convert(ComicImageView::Size s);
		ComicImageView::Size convertToSize(const QString &s);
		const QString& convert(ComicImageView::Scaling s);
		ComicImageView::Scaling convertToScaling(const QString &s);

	signals:
		void backgroundChanged(const QColor &color);
		void scalingMethodChanged(ComicImageView::Scaling s);
		void cursorChanged(bool f);

	private:
		ComicBookSettings();
		~ComicBookSettings();

	public:
		void load();

		bool smallCursor() const;
		bool twoPagesMode() const;
		bool twoPagesStep() const;
		bool japaneseMode() const;
		bool continuousScrolling() const;
		bool scrollbarsVisible() const;
		QRect geometry() const;
		ComicImageView::Size pageSize() const;
		ComicImageView::Scaling pageScaling() const;
		QString lastDir() const;
		const History& recentlyOpened() const;
		QColor background() const;
		int cacheSize() const;
		bool cacheThumbnails() const;
		int thumbnailsAge() const;
		bool preloadPages() const;
		bool confirmExit() const;
		bool autoInfo() const;
		bool fullScreenHideMenu() const;
		bool fullScreenHideStatusbar() const;
		bool showStatusbar() const;
		const QFont& infoFont() const;
		void restoreDockLayout(QMainWindow *w);
		
		void smallCursor(bool f);
		void twoPagesMode(bool f);
		void twoPagesStep(bool f);
		void japaneseMode(bool f);
		void continuousScrolling(bool f);
		void scrollbarsVisible(bool f);
		void geometry(const QRect g);
		void pageSize(ComicImageView::Size s);
		void pageScaling(ComicImageView::Scaling s);
		void lastDir(const QString &d);
		void recentlyOpened(const History &hist);
		void background(const QColor &color);
		void cacheSize(int s);
		void cacheThumbnails(bool f);
		void thumbnailsAge(int n);
		void preloadPages(bool f);
		void confirmExit(bool f);
		void autoInfo(bool f);
		void fullScreenHideMenu(bool f);
		void fullScreenHideStatusbar(bool f);
		void showStatusbar(bool f);
		void infoFont(const QFont &f);
		void saveDockLayout(QMainWindow *w);
		
		static ComicBookSettings& instance();

		//
		// checks and creates .qcomicbook/ and cache subdirectories if
		// necessary
		static bool checkDirs();
		static const QString& bookmarksDir();
		static const QString& thumbnailsDir();
};

#endif

