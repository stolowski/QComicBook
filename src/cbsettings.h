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

class QSettings;
class QRect;
class QColor;
class QMainWindow;

class ComicBookSettings: public QObject
{
	Q_OBJECT
		
	private:
		QSettings *cfg;
		bool twopages;
		bool contscroll;
		bool scrollbars;
		bool preload;
		bool fscrhidetoolbar;
		bool fscrhidemenu;
		bool fscrhidescroll;
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
		QString docklayout;
		
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

	public:
		ComicBookSettings();
		~ComicBookSettings();

		void load();

		bool getTwoPagesMode() const;
		bool getContinuousScrolling() const;
		bool getScrollbarsVisible() const;
		QRect getGeometry() const;
		ComicImageView::Size getPageSize() const;
		ComicImageView::Scaling getScaling() const;
		QString getLastDir() const;
		const History& getRecentlyOpened() const;
		QColor getBackground() const;
		int getCacheSize() const;
		bool getCacheThumbnails() const;
		int getThumbnailsAge() const;
		bool getPreload() const;
		bool getConfirmExit() const;
		bool getAutoInfo() const;
		bool getFullScreenHideToolbar() const;
		bool getFullScreenHideMenu() const;
		bool getFullScreenHideScrollbars() const;
		void restoreDockLayout(QMainWindow *w);

		void setTwoPagesMode(bool f);
		void setContinuousScrolling(bool f);
		void setScrollbarsVisible(bool f);
		void setGeometry(const QRect g);
		void setPageSize(ComicImageView::Size s);
		void setScaling(ComicImageView::Scaling s);
		void setLastDir(const QString &d);
		void setRecentlyOpened(const History &hist);
		void setBackground(const QColor &color);
		void setCacheSize(int s);
		void setCacheThumbnails(bool f);
		void setThumbnailsAge(int n);
		void setPreload(bool f);
		void setConfirmExit(bool f);
		void setAutoInfo(bool f);
		void setFullScreenHideToolbar(bool f);
		void setFullScreenHideMenu(bool f);
		void setFullScreenHideScrollbars(bool f);
		void saveDockLayout(QMainWindow *w);
		
		//
		// checks and creates .qcomicbook/ and cache subdirectories if
		// necessary
		static bool checkDirs();
		static const QString& bookmarksDir();
		static const QString& thumbnailsDir();
};

#endif

