/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ComicBookSettings.h"
#include "History.h"
#include "Utility.h"
#include "EnumMap.h"
#include <QSettings>
#include <QRect>
#include <QColor>
#include <QDir>
#include <QTextStream>
#include <QMainWindow>

#define GRP_VIEW                     "/View"
#define OPT_TWOPAGES                 "/TwoPages"
#define OPT_JAPANESEMODE             "/JapaneseMode"
#define OPT_SCROLLBARS               "/Scrollbars"
//#define OPT_SCALING                  "/Scaling"
#define OPT_PAGESIZE                 "/PageSize"
#define OPT_BACKGROUND               "/Background"
#define OPT_FULLSCREENHIDEMENU       "/FullScreenHideMenu"
#define OPT_FULLSCREENHIDESTATUS     "/FullScreenHideStatusbar"
#define OPT_FULLSCREENHIDETOOLBAR    "/FullScreenHideToolbar"
#define OPT_STATUSBAR                "/Statusbar"
#define OPT_FONT                     "/InfoFont"
#define OPT_SMALLCURSOR              "/SmallCursor"
#define OPT_EMBEDPAGENUMBERS         "/EmbedPageNumbers"

#define GRP_NAVI                     "/Navigation"
#define OPT_CONTSCROLL               "/ContinuousScroll"
#define OPT_TWOPAGESSTEP             "/TwoPagesStep"

#define GRP_WINDOW      "/Window"
#define OPT_GEOMETRY    "/Geometry"
#define OPT_DOCKLAYOUT  "/DockLayout"

#define GRP_MISC        "/Misc"
#define OPT_AUTOINFO    "/InfoDialog"
#define OPT_LASTDIR     "/LastDir"
#define OPT_RECENT      "/RecentlyOpened"
#define OPT_CACHESIZE   "/CacheSize"
#define OPT_CACHEADJUST "/CacheAutoAdjust"
#define OPT_THUMBSAGE   "/ThumbnailsAge"
#define OPT_CACHETHUMBS "/CacheThumbnails"
#define OPT_PRELOAD     "/Preload"
#define OPT_CONFIRMEXIT "/ConfirmExit"
#define OPT_SHOWSPLASH  "/ShowSplashscreen"
#define OPT_TMPDIR      "/TmpDir"

using namespace QComicBook;

QString ComicBookSettings::bkpath = QString::null;
QString ComicBookSettings::thpath = QString::null;
bool ComicBookSettings::dirsok = false;

const EnumMap<Size> ComicBookSettings::size2string[] = {
	{"original",  Original},
	{"fitwidth",  FitWidth},
	{"fitheight", FitHeight},
	{"wholepage", WholePage},
	{"bestfit",   BestFit},
	{QString::null}
};

/*const EnumMap<Scaling> ComicBookSettings::scaling2string[] = {
	{"smooth", Smooth},
	{"fast",   Fast},
	{QString::null}
};*/

ComicBookSettings& ComicBookSettings::instance()
{
	static ComicBookSettings cfg;
	return cfg;
}

ComicBookSettings::ComicBookSettings(): QObject()
{
	cfg = new QSettings();
//	cfg->insertSearchPath(QSettings::Unix, QDir::homeDirPath() + "/.qcomicbook");
	cfg->beginGroup("/QComicBook");
}

ComicBookSettings::~ComicBookSettings()
{
	cfg->endGroup();
	delete cfg;
}

bool ComicBookSettings::checkDirs()
{
	dirsok = false; 
	bkpath = QDir::homePath() + "/.qcomicbook";
	QDir dir(bkpath);
	if (!dir.exists())
		if (!dir.mkdir(bkpath))
			return false;
	
	dir.setPath(thpath = bkpath + "/cache");
	if (!dir.exists())
		if (!dir.mkdir(thpath))
			return false;
	return dirsok = true;
}

const QString& ComicBookSettings::bookmarksDir()
{
	return bkpath;
}

const QString& ComicBookSettings::thumbnailsDir()
{
	return thpath;
}

void ComicBookSettings::load()
{
	QString defbrowser(QString::null);
	QString fontdesc;

	const char *browsers[] = {"firefox", "mozilla", "konqueror", "opera", NULL};
	/*for (int i=0; browsers[i]; i++) FIXME
	{
		if (QString path = QComicBook::which(browsers[i]))
		{
			defbrowser = path;
			break;
		}
	}*/
	
	cfg->beginGroup(GRP_WINDOW);
		geometry = cfg->value(OPT_GEOMETRY).toByteArray();
		docklayout = cfg->value(OPT_DOCKLAYOUT).toByteArray();
	cfg->endGroup();
	cfg->beginGroup(GRP_VIEW);
                embedpagenumbers = cfg->value(OPT_EMBEDPAGENUMBERS, false).toBool();
		smallcursor = cfg->value(OPT_SMALLCURSOR, false).toBool();
		twopages = cfg->value(OPT_TWOPAGES, false).toBool();
		japanese = cfg->value(OPT_JAPANESEMODE, false).toBool();
		scrollbars = cfg->value(OPT_SCROLLBARS, false).toBool();
		//scaling = convert(scaling2string, cfg->readEntry(OPT_SCALING, size2string[0].str));
		pagesize = convert(size2string, cfg->value(OPT_PAGESIZE, size2string[0].str).toString());
		bgcolor = cfg->value(OPT_BACKGROUND).value<QColor>();
		fscrhidemenu = cfg->value(OPT_FULLSCREENHIDEMENU, true).toBool();
		fscrhidestatus = cfg->value(OPT_FULLSCREENHIDESTATUS, true).toBool();
		fscrhidetoolbar = cfg->value(OPT_FULLSCREENHIDETOOLBAR, false).toBool();
		statusbar = cfg->value(OPT_STATUSBAR, true).toBool();
		fontdesc = cfg->value(OPT_FONT, QString()).toString();
		if (fontdesc.isNull() || !font.fromString(fontdesc))
		{
			font.setFamily("Courier");
			font.setPointSize(10);
		}
	cfg->endGroup();
	cfg->beginGroup(GRP_NAVI);
		contscroll = cfg->value(OPT_CONTSCROLL, true).toBool();
		twopagesstep = cfg->value(OPT_TWOPAGESSTEP, true).toBool();
	cfg->endGroup();
	cfg->beginGroup(GRP_MISC);
		lastdir = cfg->value(OPT_LASTDIR, QString()).toString();
		recent = cfg->value(OPT_RECENT).toStringList(); 
		cachesize = cfg->value(OPT_CACHESIZE, 3).toInt();
		if (cachesize < 1)
			cachesize = 1;
		cacheadjust = cfg->value(OPT_CACHEADJUST, true).toBool();
		preload = cfg->value(OPT_PRELOAD, true).toBool();
		confirmexit = cfg->value(OPT_CONFIRMEXIT, true).toBool();
		autoinfo = cfg->value(OPT_AUTOINFO, false).toBool();
		showsplash = cfg->value(OPT_SHOWSPLASH, true).toBool();
		thumbsage = cfg->value(OPT_THUMBSAGE, 7).toInt();
		cachethumbs = cfg->value(OPT_CACHETHUMBS, true).toBool();
		tmpdir = cfg->value(OPT_TMPDIR, QString()).toString();
		QDir dir(tmpdir);
		if (tmpdir.isNull() || !dir.exists())
			tmpdir = "/tmp";
	cfg->endGroup();
}

bool ComicBookSettings::embedPageNumbers() const
{
    return embedpagenumbers;
}

bool ComicBookSettings::smallCursor() const
{
	return smallcursor;
}

bool ComicBookSettings::twoPagesMode() const
{
	return twopages;
}

bool ComicBookSettings::twoPagesStep() const
{
	return twopagesstep;
}

bool ComicBookSettings::japaneseMode() const
{
	return japanese;
}

bool ComicBookSettings::continuousScrolling() const
{
	return contscroll;
}

bool ComicBookSettings::scrollbarsVisible() const
{
	return scrollbars;
}

void ComicBookSettings::restoreGeometry(QMainWindow *w) const
{
	w->restoreGeometry(geometry);
}

Size ComicBookSettings::pageSize() const
{
	return pagesize;
}

/*Scaling ComicBookSettings::pageScaling() const
{
	return scaling;
}*/

QString ComicBookSettings::lastDir() const
{
	return lastdir;
}

const History& ComicBookSettings::recentlyOpened() const
{
	return recent;
}

QColor ComicBookSettings::background() const
{
	return bgcolor;
}

int ComicBookSettings::cacheSize() const
{
	return cachesize;
}

bool ComicBookSettings::cacheAutoAdjust() const
{
	return cacheadjust;
}

bool ComicBookSettings::cacheThumbnails() const
{
	return cachethumbs;
}

int ComicBookSettings::thumbnailsAge() const
{
	return thumbsage;
}

bool ComicBookSettings::preloadPages() const
{
	return preload;
}

bool ComicBookSettings::confirmExit() const
{
	return confirmexit;
}

bool ComicBookSettings::autoInfo() const
{
	return autoinfo;
}

bool ComicBookSettings::fullScreenHideMenu() const
{
	return fscrhidemenu;
}

bool ComicBookSettings::fullScreenHideStatusbar() const
{
	return fscrhidestatus;
}

bool ComicBookSettings::fullScreenHideToolbar() const
{
	return fscrhidetoolbar;
}

bool ComicBookSettings::showStatusbar() const
{
	return statusbar;
}

const QFont& ComicBookSettings::infoFont() const
{
	return font;
}

bool ComicBookSettings::useInternalBrowser() const
{
	return intbrowser;
}

QString ComicBookSettings::externalBrowser() const
{
	return extbrowser;
}

bool ComicBookSettings::showSplash() const
{
	return showsplash;
}

void ComicBookSettings::restoreDockLayout(QMainWindow *w)
{
	w->restoreState(docklayout);
}

QString ComicBookSettings::tmpDir() const
{
	return tmpdir;
}

void ComicBookSettings::embedPageNumbers(bool f)
{
	if (f != embedpagenumbers)
	{
		cfg->setValue(GRP_VIEW OPT_EMBEDPAGENUMBERS, embedpagenumbers = f);
                emit displaySettingsChanged();
	}
}

void ComicBookSettings::smallCursor(bool f)
{
	if (f != smallcursor)
	{
		cfg->setValue(GRP_VIEW OPT_SMALLCURSOR, smallcursor = f);
		emit displaySettingsChanged();
	}
}

void ComicBookSettings::twoPagesMode(bool f)
{
	if (f != twopages)
		cfg->setValue(GRP_VIEW OPT_TWOPAGES, twopages = f);
}

void ComicBookSettings::twoPagesStep(bool f)
{
	if (f != twopagesstep)
		cfg->setValue(GRP_NAVI OPT_TWOPAGESSTEP, twopagesstep = f);
}

void ComicBookSettings::japaneseMode(bool f)
{
	if (f != japanese)
		cfg->setValue(GRP_VIEW OPT_JAPANESEMODE, japanese = f);
}

void ComicBookSettings::continuousScrolling(bool f)
{
	if (f != contscroll)
		cfg->setValue(GRP_NAVI OPT_CONTSCROLL, contscroll = f);
}

void ComicBookSettings::scrollbarsVisible(bool f)
{
	if (f != scrollbars)
		cfg->setValue(GRP_VIEW OPT_SCROLLBARS, scrollbars = f);
}

void ComicBookSettings::saveGeometry(QMainWindow *w)
{
	cfg->setValue(GRP_WINDOW OPT_GEOMETRY, w->saveGeometry());
}

void ComicBookSettings::pageSize(Size s)
{
	if (s != pagesize)
		cfg->setValue(GRP_VIEW OPT_PAGESIZE, convert(size2string, pagesize = s));
}

/*void ComicBookSettings::pageScaling(Scaling s)
{
	if (s != scaling)
	{
		cfg->setValue(GRP_VIEW OPT_SCALING, convert(scaling2string, scaling = s));
		emit scalingMethodChanged(scaling);
	}
}*/

void ComicBookSettings::lastDir(const QString &d)
{
	if (lastdir != d)
		cfg->setValue(GRP_MISC OPT_LASTDIR, lastdir = d);
}

void ComicBookSettings::recentlyOpened(const History &hist)
{
	recent = hist;
	cfg->setValue(GRP_MISC OPT_RECENT, recent.getAll());
}

void ComicBookSettings::background(const QColor &color)
{
	if (color != bgcolor)
	{
		bgcolor = color;
		cfg->setValue(GRP_VIEW OPT_BACKGROUND, bgcolor.name());
		emit displaySettingsChanged();
	}
}

void ComicBookSettings::cacheSize(int s)
{
	if (s != cachesize)
	{
		if (s < 1)
			s = 1;
		cfg->setValue(GRP_MISC OPT_CACHESIZE, cachesize = s);
	}
}

void ComicBookSettings::cacheAutoAdjust(bool f)
{
	if (f != cacheadjust)
	{
		cacheadjust = f;
		cfg->setValue(GRP_MISC OPT_CACHEADJUST, cacheadjust = f);
	}
}

void ComicBookSettings::cacheThumbnails(bool f)
{
	if (f != cachethumbs)
		cfg->setValue(GRP_MISC OPT_CACHETHUMBS, cachethumbs = f);
}

void ComicBookSettings::thumbnailsAge(int n)
{
	if (n != thumbsage)
		cfg->setValue(GRP_MISC OPT_THUMBSAGE, thumbsage = n);
}

void ComicBookSettings::preloadPages(bool f)
{
	if (f != preload)
		cfg->setValue(GRP_MISC OPT_PRELOAD, preload = f);
}

void ComicBookSettings::confirmExit(bool f)
{
	if (f != confirmexit)
		cfg->setValue(GRP_MISC OPT_CONFIRMEXIT, confirmexit = f);
}

void ComicBookSettings::autoInfo(bool f)
{
	if (f != autoinfo)
		cfg->setValue(GRP_MISC OPT_AUTOINFO, autoinfo = f);
}

void ComicBookSettings::fullScreenHideMenu(bool f)
{
	if (f != fscrhidemenu)
		cfg->setValue(GRP_VIEW OPT_FULLSCREENHIDEMENU, fscrhidemenu = f);
}

void ComicBookSettings::fullScreenHideStatusbar(bool f)
{
	if (f != fscrhidestatus)
		cfg->setValue(GRP_VIEW OPT_FULLSCREENHIDESTATUS, fscrhidestatus = f);
}

void ComicBookSettings::fullScreenHideToolbar(bool f)
{
	if (f != fscrhidetoolbar)
		cfg->setValue(GRP_VIEW OPT_FULLSCREENHIDETOOLBAR, fscrhidetoolbar = f);
}

void ComicBookSettings::showStatusbar(bool f)
{
	if (f != statusbar)
		cfg->setValue(GRP_VIEW OPT_STATUSBAR, statusbar = f);
}

void ComicBookSettings::infoFont(const QFont &s)
{
	if (s != font)
	{
		font = s;
		cfg->setValue(GRP_VIEW OPT_FONT, font.toString());
	}
}

void ComicBookSettings::saveDockLayout(QMainWindow *w)
{
	cfg->setValue(GRP_WINDOW OPT_DOCKLAYOUT, w->saveState());
}

void ComicBookSettings::showSplash(bool f)
{
	if (f != showsplash)
		cfg->setValue(GRP_MISC OPT_SHOWSPLASH, showsplash = f);
}

void ComicBookSettings::tmpDir(const QString &dir)
{
	if (dir != tmpdir)
		cfg->setValue(GRP_MISC OPT_TMPDIR, tmpdir = dir);
}

