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
#include "miscutil.h"
#include "enummap.h"
#include <qrect.h>
#include <qcolor.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qmainwindow.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#define GRP_WINDOW      "/Window"
#define OPT_X           "/X"
#define OPT_Y           "/Y"
#define OPT_WIDTH       "/Width"
#define OPT_HEIGHT      "/Height"
#define OPT_DOCKLAYOUT  "/DockLayout"

#define OPT_LASTDIR     "/LastDir"

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

const EnumMap<Scaling> ComicBookSettings::scaling2string[] = {
	{"smooth", Smooth},
	{"fast",   Fast},
	{QString::null}
};

ComicBookSettings& ComicBookSettings::instance()
{
	static ComicBookSettings cfg;
	return cfg;
}

ComicBookSettings::ComicBookSettings(): KConfigSkeleton()
{
	setCurrentGroup("View");
	addItemBool("TwoPages", twopages, false);
	addItemBool("JapaneseMode", japanese, false);
	addItemBool("Scrollbars", scrollbars, false);
	addItemBool("FullScreenHideMenu", fscrhidemenu, false);
	addItemBool("FullScreenHideStatusbar", fscrhidestatus, true);
	addItemBool("Statusbar", statusbar, true);
	addItemBool("SmallCursor", smallcursor, false);
	addItemColor("Background", bgcolor, QColor("#000000"));
	addItemFont("InfoFont", font);
	addItemString("Scaling", scaling, scaling2string[0].str);
	addItemString("PageSize", pagesize, size2string[0].str);

	setCurrentGroup("Navigation");
	addItemBool("ContinuousScroll", contscroll, true);
	addItemBool("TwoPagesStep", twopagesstep, true);
	
	setCurrentGroup("Misc");
	addItemBool("Preload", preload, true);
	addItemBool("ConfirmExit", preload, true);
	addItemBool("InfoDialog",  autoinfo, false);
	addItemBool("SplashScreen", splashscreen, true);
	addItemBool("EditSupport", editsupport, false);
	addItemInt("CacheSize", cachesize, 3);
	addItemInt("ThumbnailsAge", thumbsage, 7);
	addItemBool("CacheThumbnails", cachethumbs, false);
	addItemStringList("RecentlyOpened", recent);
}

ComicBookSettings::~ComicBookSettings()
{
}

bool ComicBookSettings::checkDirs()
{
	bkpath = KGlobal::dirs()->saveLocation("data", "qcomicbok/", true);	
	thpath = KGlobal::dirs()->saveLocation("data", "qcomicbok/cache/", true);
	dirsok = (bkpath != QString::null && thpath != QString::null);
	return dirsok;
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
/*
	cfg->beginGroup(GRP_WINDOW);
		x = cfg->readNumEntry(OPT_X, 0);
		y = cfg->readNumEntry(OPT_Y, 0);
		w = cfg->readNumEntry(OPT_WIDTH, 640);
		h = cfg->readNumEntry(OPT_HEIGHT, 400);
		docklayout = cfg->readEntry(OPT_DOCKLAYOUT, QString::null);
	cfg->endGroup();
	cfg->beginGroup(GRP_MISC);
		lastdir = cfg->readEntry(OPT_LASTDIR, QString::null);
	cfg->endGroup();*/
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

Size ComicBookSettings::pageSize() const
{
	return convert(size2string, pagesize);
}

Scaling ComicBookSettings::pageScaling() const
{
	return convert(scaling2string, scaling);
}

QString ComicBookSettings::lastDir() const
{
	return lastdir;
}

const QStringList& ComicBookSettings::recentlyOpened() const
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

bool ComicBookSettings::showSplashScreen() const
{
	return splashscreen;
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

bool ComicBookSettings::editSupport() const
{
	return editsupport;
}

void ComicBookSettings::smallCursor(bool f)
{
	smallcursor = f;
	//emit cursorChanged(f);
}

void ComicBookSettings::twoPagesMode(bool f)
{
	twopages = f;
}

void ComicBookSettings::twoPagesStep(bool f)
{
	twopagesstep = f;
}

void ComicBookSettings::japaneseMode(bool f)
{
	japanese = f;
}

void ComicBookSettings::continuousScrolling(bool f)
{
	contscroll = f;
}

void ComicBookSettings::scrollbarsVisible(bool f)
{
	scrollbars = f;
}

void ComicBookSettings::geometry(const QRect g)
{
	if (x != g.x() || y != g.y() || w != g.width() || h != g.height())
	{
		/*cfg->beginGroup(GRP_WINDOW);
		cfg->writeEntry(OPT_X, x = g.x());
		cfg->writeEntry(OPT_Y, y = g.y());
		cfg->writeEntry(OPT_WIDTH, w = g.width());
		cfg->writeEntry(OPT_HEIGHT, h = g.height());
		cfg->endGroup();*/
	}
}

void ComicBookSettings::pageSize(Size s)
{
	pagesize = convert(size2string, s);
}

void ComicBookSettings::pageScaling(Scaling s)
{
	scaling = convert(scaling2string, s);
	//emit scalingMethodChanged(scaling);
}

void ComicBookSettings::lastDir(const QString &d)
{
	//cfg->writeEntry(GRP_MISC OPT_LASTDIR, lastdir = d);
}

void ComicBookSettings::recentlyOpened(const QStringList &hist)
{
	recent = hist;
}

void ComicBookSettings::background(const QColor &color)
{
	bgcolor = color;
}

void ComicBookSettings::cacheSize(int s)
{
	cachesize = s;
}

void ComicBookSettings::cacheThumbnails(bool f)
{
	cachethumbs = f;
}

void ComicBookSettings::thumbnailsAge(int n)
{
	thumbsage = n;
}

void ComicBookSettings::preloadPages(bool f)
{
	preload = f;
}

void ComicBookSettings::confirmExit(bool f)
{
	confirmexit = f;
}

void ComicBookSettings::autoInfo(bool f)
{
	autoinfo = f;
}

void ComicBookSettings::fullScreenHideMenu(bool f)
{
	fscrhidemenu = f;
}

void ComicBookSettings::fullScreenHideStatusbar(bool f)
{
	fscrhidestatus = f;
}

void ComicBookSettings::showStatusbar(bool f)
{
	statusbar = f;
}

void ComicBookSettings::showSplashScreen(bool f)
{
	splashscreen = f;
}

void ComicBookSettings::infoFont(const QFont &s)
{
	font = s;
}

void ComicBookSettings::saveDockLayout(QMainWindow *w)
{
	QString tmp;
	QTextStream str(&tmp, IO_WriteOnly);
	str << *w;
	//cfg->writeEntry(GRP_WINDOW OPT_DOCKLAYOUT, tmp);
}

void ComicBookSettings::editSupport(bool f)
{
	editsupport = f;
}

//#include "cbsettings.moc"
