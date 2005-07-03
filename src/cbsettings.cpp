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

#include "cbsettings.h"
#include "history.h"
#include <qsettings.h>
#include <qrect.h>
#include <qcolor.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qmainwindow.h>

#define GRP_VIEW                     "/View"
#define OPT_TWOPAGES                 "/TwoPages"
#define OPT_JAPANESEMODE             "/JapaneseMode"
#define OPT_SCROLLBARS               "/Scrollbars"
#define OPT_SCALING                  "/Scaling"
#define OPT_PAGESIZE                 "/PageSize"
#define OPT_BACKGROUND               "/Background"
#define OPT_FULLSCREENHIDEMENU       "/FullScreenHideMenu"
#define OPT_FULLSCREENHIDESTATUS     "/FullScreenHideStatusbar"
#define OPT_STATUSBAR                "/Statusbar"
#define OPT_FONT                     "/InfoFont"
#define OPT_SMALLCURSOR              "/SmallCursor"

#define GRP_NAVI                     "/Navigation"
#define OPT_CONTSCROLL               "/ContinuousScroll"
#define OPT_TWOPAGESSTEP             "/TwoPagesStep"

#define GRP_WINDOW      "/Window"
#define OPT_X           "/X"
#define OPT_Y           "/Y"
#define OPT_WIDTH       "/Width"
#define OPT_HEIGHT      "/Height"
#define OPT_DOCKLAYOUT  "/DockLayout"

#define GRP_MISC        "/Misc"
#define OPT_AUTOINFO    "/InfoDialog"
#define OPT_LASTDIR     "/LastDir"
#define OPT_RECENT      "/RecentlyOpened"
#define OPT_CACHESIZE   "/CacheSize"
#define OPT_THUMBSAGE   "/ThumbnailsAge"
#define OPT_CACHETHUMBS "/CacheThumbnails"
#define OPT_PRELOAD     "/Preload"
#define OPT_CONFIRMEXIT "/ConfirmExit"

QString ComicBookSettings::bkpath = QString::null;
QString ComicBookSettings::thpath = QString::null;
bool ComicBookSettings::dirsok = false;

const ComicBookSettings::int2qstring ComicBookSettings::size2string[] = {
	{ComicImageView::Original,  "original"},
	{ComicImageView::FitWidth,  "fitwidth"},
	{ComicImageView::FitHeight, "fitheight"},
	{ComicImageView::WholePage, "wholepage"},
	{ComicImageView::BestFit,   "bestfit"}
};

const ComicBookSettings::int2qstring ComicBookSettings::scaling2string[] = {
	{ComicImageView::Smooth, "smooth"},
	{ComicImageView::Fast,   "fast"}
};

ComicBookSettings& ComicBookSettings::instance()
{
	static ComicBookSettings cfg;
	return cfg;
}

ComicBookSettings::ComicBookSettings(): QObject()
{
	cfg = new QSettings();
	cfg->insertSearchPath(QSettings::Unix, QDir::homeDirPath() + "/.qcomicbook");
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
	bkpath = QDir::homeDirPath() + "/.qcomicbook";
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

const QString& ComicBookSettings::convert(ComicImageView::Size s)
{
	for (int i=0; i<sizeof(size2string)/sizeof(int2qstring); i++)
		if (size2string[i].v == s)
			return size2string[i].str;
	return size2string[0].str;
}

ComicImageView::Size ComicBookSettings::convertToSize(const QString &s)
{
	for (int i=0; i<sizeof(size2string)/sizeof(int2qstring); i++)
		if (size2string[i].str == s)
			return static_cast<ComicImageView::Size>(size2string[i].v);
	return ComicImageView::Original;
}

const QString& ComicBookSettings::convert(ComicImageView::Scaling s)
{
	for (int i=0; i<sizeof(scaling2string)/sizeof(int2qstring); i++)
		if (scaling2string[i].v == s)
			return scaling2string[i].str;
	return scaling2string[0].str;
}

ComicImageView::Scaling ComicBookSettings::convertToScaling(const QString &s)
{
	for (int i=0; i<sizeof(scaling2string)/sizeof(int2qstring); i++)
		if (scaling2string[i].str == s)
			return static_cast<ComicImageView::Scaling>(scaling2string[i].v);
	return ComicImageView::Smooth;
}

void ComicBookSettings::load()
{
	QString fontdesc;
	
	cfg->beginGroup(GRP_WINDOW);
		x = cfg->readNumEntry(OPT_X, 0);
		y = cfg->readNumEntry(OPT_Y, 0);
		w = cfg->readNumEntry(OPT_WIDTH, 640);
		h = cfg->readNumEntry(OPT_HEIGHT, 400);
		docklayout = cfg->readEntry(OPT_DOCKLAYOUT, QString::null);
	cfg->endGroup();
	cfg->beginGroup(GRP_VIEW);
		smallcursor = cfg->readBoolEntry(OPT_SMALLCURSOR, false);
		twopages = cfg->readBoolEntry(OPT_TWOPAGES, false);
		japanese = cfg->readBoolEntry(OPT_JAPANESEMODE, false);
		scrollbars = cfg->readBoolEntry(OPT_SCROLLBARS, false);
		scaling = convertToScaling(cfg->readEntry(OPT_SCALING, size2string[0].str));
		pagesize = convertToSize(cfg->readEntry(OPT_PAGESIZE, size2string[0].str));
		bgcolor.setNamedColor(cfg->readEntry(OPT_BACKGROUND, "#000000"));
		fscrhidemenu = cfg->readBoolEntry(OPT_FULLSCREENHIDEMENU, true);
		fscrhidestatus = cfg->readBoolEntry(OPT_FULLSCREENHIDESTATUS, true);
		statusbar = cfg->readBoolEntry(OPT_STATUSBAR, true);
		fontdesc = cfg->readEntry(OPT_FONT, QString::null);
		if (fontdesc.isNull() || !font.fromString(fontdesc))
		{
			font.setFamily("Courier");
			font.setPointSize(10);
		}
	cfg->endGroup();
	cfg->beginGroup(GRP_NAVI);
		contscroll = cfg->readBoolEntry(OPT_CONTSCROLL, true);
		twopagesstep = cfg->readBoolEntry(OPT_TWOPAGESSTEP, true);
	cfg->endGroup();
	cfg->beginGroup(GRP_MISC);
		lastdir = cfg->readEntry(OPT_LASTDIR, QString::null);
		recent = cfg->readListEntry(OPT_RECENT);
		cachesize = cfg->readNumEntry(OPT_CACHESIZE, 3);
		preload = cfg->readBoolEntry(OPT_PRELOAD, true);
		confirmexit = cfg->readBoolEntry(OPT_CONFIRMEXIT, true);
		autoinfo = cfg->readBoolEntry(OPT_AUTOINFO, false);
		thumbsage = cfg->readNumEntry(OPT_THUMBSAGE, 7);
		cachethumbs = cfg->readBoolEntry(OPT_CACHETHUMBS, true);
	cfg->endGroup();
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

QRect ComicBookSettings::geometry() const
{
	return QRect(x, y, w, h);
}

ComicImageView::Size ComicBookSettings::pageSize() const
{
	return pagesize;
}

ComicImageView::Scaling ComicBookSettings::pageScaling() const
{
	return scaling;
}

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

bool ComicBookSettings::showStatusbar() const
{
	return statusbar;
}

const QFont& ComicBookSettings::infoFont() const
{
	return font;
}

void ComicBookSettings::restoreDockLayout(QMainWindow *w)
{
	QTextStream str(&docklayout, IO_ReadOnly);
	str >> *w;
}

void ComicBookSettings::smallCursor(bool f)
{
	if (f != smallcursor)
	{
		cfg->writeEntry(GRP_VIEW OPT_SMALLCURSOR, smallcursor = f);
		emit cursorChanged(f);
	}
}

void ComicBookSettings::twoPagesMode(bool f)
{
	if (f != twopages)
		cfg->writeEntry(GRP_VIEW OPT_TWOPAGES, twopages = f);
}

void ComicBookSettings::twoPagesStep(bool f)
{
	if (f != twopagesstep)
		cfg->writeEntry(GRP_NAVI OPT_TWOPAGESSTEP, twopagesstep = f);
}

void ComicBookSettings::japaneseMode(bool f)
{
	if (f != japanese)
		cfg->writeEntry(GRP_VIEW OPT_JAPANESEMODE, japanese = f);
}

void ComicBookSettings::continuousScrolling(bool f)
{
	if (f != contscroll)
		cfg->writeEntry(GRP_NAVI OPT_CONTSCROLL, contscroll = f);
}

void ComicBookSettings::scrollbarsVisible(bool f)
{
	if (f != scrollbars)
		cfg->writeEntry(GRP_VIEW OPT_SCROLLBARS, scrollbars = f);
}

void ComicBookSettings::geometry(const QRect g)
{
	if (x != g.x() || y != g.y() || w != g.width() || h != g.height())
	{
		cfg->beginGroup(GRP_WINDOW);
		cfg->writeEntry(OPT_X, x = g.x());
		cfg->writeEntry(OPT_Y, y = g.y());
		cfg->writeEntry(OPT_WIDTH, w = g.width());
		cfg->writeEntry(OPT_HEIGHT, h = g.height());
		cfg->endGroup();
	}
}

void ComicBookSettings::pageSize(ComicImageView::Size s)
{
	if (s != pagesize)
		cfg->writeEntry(GRP_VIEW OPT_PAGESIZE, convert(pagesize = s));
}

void ComicBookSettings::pageScaling(ComicImageView::Scaling s)
{
	if (s != scaling)
	{
		cfg->writeEntry(GRP_VIEW OPT_SCALING, convert(scaling = s));
		emit scalingMethodChanged(scaling);
	}
}

void ComicBookSettings::lastDir(const QString &d)
{
	if (lastdir != d)
		cfg->writeEntry(GRP_MISC OPT_LASTDIR, lastdir = d);
}

void ComicBookSettings::recentlyOpened(const History &hist)
{
	recent = hist;
	cfg->writeEntry(GRP_MISC OPT_RECENT, recent.getAll());
}

void ComicBookSettings::background(const QColor &color)
{
	if (color != bgcolor)
	{
		bgcolor = color;
		cfg->writeEntry(GRP_VIEW OPT_BACKGROUND, bgcolor.name());
		emit backgroundChanged(bgcolor);
	}
}

void ComicBookSettings::cacheSize(int s)
{
	if (s != cachesize)
		cfg->writeEntry(GRP_MISC OPT_CACHESIZE, cachesize = s);
}

void ComicBookSettings::cacheThumbnails(bool f)
{
	if (f != cachethumbs)
		cfg->writeEntry(GRP_MISC OPT_CACHETHUMBS, cachethumbs = f);
}

void ComicBookSettings::thumbnailsAge(int n)
{
	if (n != thumbsage)
		cfg->writeEntry(GRP_MISC OPT_THUMBSAGE, thumbsage = n);
}

void ComicBookSettings::preloadPages(bool f)
{
	if (f != preload)
		cfg->writeEntry(GRP_MISC OPT_PRELOAD, preload = f);
}

void ComicBookSettings::confirmExit(bool f)
{
	if (f != confirmexit)
		cfg->writeEntry(GRP_MISC OPT_CONFIRMEXIT, confirmexit = f);
}

void ComicBookSettings::autoInfo(bool f)
{
	if (f != autoinfo)
		cfg->writeEntry(GRP_MISC OPT_AUTOINFO, autoinfo = f);
}

void ComicBookSettings::fullScreenHideMenu(bool f)
{
	if (f != fscrhidemenu)
		cfg->writeEntry(GRP_VIEW OPT_FULLSCREENHIDEMENU, fscrhidemenu = f);
}

void ComicBookSettings::fullScreenHideStatusbar(bool f)
{
	if (f != fscrhidestatus)
		cfg->writeEntry(GRP_VIEW OPT_FULLSCREENHIDESTATUS, fscrhidestatus = f);
}

void ComicBookSettings::showStatusbar(bool f)
{
	if (f != statusbar)
		cfg->writeEntry(GRP_VIEW OPT_STATUSBAR, statusbar = f);
}

void ComicBookSettings::infoFont(const QFont &s)
{
	if (s != font)
	{
		font = s;
		cfg->writeEntry(GRP_VIEW OPT_FONT, font.toString());
	}
}

void ComicBookSettings::saveDockLayout(QMainWindow *w)
{
	QString tmp;
	QTextStream str(&tmp, IO_WriteOnly);
	str << *w;
	cfg->writeEntry(GRP_WINDOW OPT_DOCKLAYOUT, tmp);
}

