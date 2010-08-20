/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
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
#include "ComicMainWindow.h"
#include <QSettings>
#include <QColor>
#include <QDir>
#include <QTextStream>
#include <QDesktopServices>
#include <iostream>

#define GRP_VIEW                     "/View"
#define OPT_TWOPAGES                 "/TwoPages"
#define OPT_JAPANESEMODE             "/JapaneseMode"
#define OPT_SCROLLBARS               "/Scrollbars"
#define OPT_SMOOTHSCALING            "/SmoothScaling"
#define OPT_PAGESIZE                 "/PageSize"
#define OPT_BACKGROUND               "/Background"
#define OPT_FULLSCREENHIDEMENU       "/FullScreenHideMenu"
#define OPT_FULLSCREENHIDESTATUS     "/FullScreenHideStatusbar"
#define OPT_FULLSCREENHIDETOOLBAR    "/FullScreenHideToolbar"
#define OPT_STATUSBAR                "/Statusbar"
#define OPT_FONT                     "/InfoFont"
#define OPT_SMALLCURSOR              "/SmallCursor"
#define OPT_EMBEDPAGENUMBERS         "/EmbedPageNumbers"
#define OPT_CONTSCROLL               "/ContinuousScroll"
#define OPT_VIEWTYPE                 "/ViewType"

#define GRP_NAVI                     "/Navigation"

#define GRP_WINDOW      "/Window"
#define OPT_GEOMETRY    "/Geometry"
#define OPT_DOCKLAYOUT  "/DockLayout"

#define GRP_RUNTIME     "/Runtime"
#define OPT_LASTDIR     "/LastDir"
#define OPT_RECENT      "/RecentlyOpened"

#define GRP_MISC        "/Misc"
#define OPT_AUTOINFO    "/InfoDialog"
#define OPT_CACHESIZE   "/CacheSize"
#define OPT_CACHEADJUST "/CacheAutoAdjust"
#define OPT_THUMBSAGE   "/ThumbnailsAge"
#define OPT_CACHETHUMBS "/CacheThumbnails"
#define OPT_PRELOAD     "/Preload"
#define OPT_CONFIRMEXIT "/ConfirmExit"
#define OPT_SHOWSPLASH  "/ShowSplashscreen"
#define OPT_TMPDIR      "/TmpDir"

using namespace QComicBook;

const EnumMap<Size> ComicBookSettings::size2string[] = {
	{"original",  Original},
	{"fitwidth",  FitWidth},
	{"fitheight", FitHeight},
	{"wholepage", WholePage},
	{"bestfit",   BestFit},
	{QString::null}
};

const EnumMap<ViewType> ComicBookSettings::viewtype2string[] = {
	{"simple",  Simple},
	{"continuous", Continuous},
	{"frame", Frame},
	{QString::null}
};

ComicBookSettings& ComicBookSettings::instance()
{
    static ComicBookSettings cfg;
    return cfg;
}

ComicBookSettings::ComicBookSettings(): QObject()
                                      , m_bkpath(QString::null)
                                      , m_thpath(QString::null)
                                      , m_dirsok(false)
{
	m_cfg = new QSettings();
//	cfg->insertSearchPath(QSettings::Unix, QDir::homeDirPath() + "/.qcomicbook");
	m_cfg->beginGroup("/QComicBook");
}

ComicBookSettings::~ComicBookSettings()
{
	m_cfg->endGroup();
	delete m_cfg;
}

bool ComicBookSettings::checkDirs()
{
	m_dirsok = false; 
	m_bkpath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

	QDir dir(m_bkpath);
	if (!dir.exists())
        {
            if (!dir.mkpath(m_bkpath))
            {
                return false;
            }
        }

        m_thpath = QDesktopServices::storageLocation(QDesktopServices::CacheLocation) + QDir::separator() + "thumbs";

        dir.setPath(m_thpath);
	if (!dir.exists())
        {
            if (!dir.mkpath(m_thpath))
            {
                return false;
            }
        }
	return m_dirsok = true;
}

const QString& ComicBookSettings::bookmarksDir()
{
	return m_bkpath;
}

const QString& ComicBookSettings::thumbnailsDir()
{
	return m_thpath;
}

void ComicBookSettings::load()
{
	QString fontdesc;
	m_cfg->beginGroup(GRP_VIEW);
                m_embedpagenumbers = m_cfg->value(OPT_EMBEDPAGENUMBERS, false).toBool();
		m_smallcursor = m_cfg->value(OPT_SMALLCURSOR, false).toBool();
		m_twopages = m_cfg->value(OPT_TWOPAGES, false).toBool();
		m_japanese = m_cfg->value(OPT_JAPANESEMODE, false).toBool();
		m_scrollbars = m_cfg->value(OPT_SCROLLBARS, false).toBool();
		m_smoothscaling = m_cfg->value(OPT_SMOOTHSCALING, true).toBool();
		m_pagesize = convert(size2string, m_cfg->value(OPT_PAGESIZE, size2string[0].str).toString());
		m_bgcolor = m_cfg->value(OPT_BACKGROUND).value<QColor>();
		m_fscrhidemenu = m_cfg->value(OPT_FULLSCREENHIDEMENU, true).toBool();
		m_fscrhidestatus = m_cfg->value(OPT_FULLSCREENHIDESTATUS, true).toBool();
		m_fscrhidetoolbar = m_cfg->value(OPT_FULLSCREENHIDETOOLBAR, false).toBool();
		m_statusbar = m_cfg->value(OPT_STATUSBAR, true).toBool();
		fontdesc = m_cfg->value(OPT_FONT, QString()).toString();
		if (fontdesc.isNull() || !m_font.fromString(fontdesc))
		{
			m_font.setFamily("Courier");
			m_font.setPointSize(10);
		}
		m_contscroll = m_cfg->value(OPT_CONTSCROLL, true).toBool();
		m_viewtype = convert(viewtype2string, m_cfg->value(OPT_VIEWTYPE, viewtype2string[0].str).toString());
	m_cfg->endGroup();
	m_cfg->beginGroup(GRP_RUNTIME);
		m_lastdir = m_cfg->value(OPT_LASTDIR, QString()).toString();
		m_recent = m_cfg->value(OPT_RECENT).toStringList(); 
        m_cfg->endGroup();
	m_cfg->beginGroup(GRP_MISC);
		m_cachesize = m_cfg->value(OPT_CACHESIZE, 3).toInt();
		if (m_cachesize < 1)
                {
                    m_cachesize = 1;
                }
		m_cacheadjust = m_cfg->value(OPT_CACHEADJUST, true).toBool();
		m_preload = m_cfg->value(OPT_PRELOAD, true).toBool();
		m_confirmexit = m_cfg->value(OPT_CONFIRMEXIT, true).toBool();
		m_autoinfo = m_cfg->value(OPT_AUTOINFO, false).toBool();
		m_showsplash = m_cfg->value(OPT_SHOWSPLASH, true).toBool();
		m_thumbsage = m_cfg->value(OPT_THUMBSAGE, 7).toInt();
		m_cachethumbs = m_cfg->value(OPT_CACHETHUMBS, true).toBool();
		m_tmpdir = m_cfg->value(OPT_TMPDIR, QString()).toString();
		QDir dir(m_tmpdir);
		if (m_tmpdir.isNull() || !dir.exists())
                {
                    m_tmpdir = QDir::tempPath();
                }
	m_cfg->endGroup();
}

bool ComicBookSettings::embedPageNumbers() const
{
    return m_embedpagenumbers;
}

bool ComicBookSettings::smallCursor() const
{
    return m_smallcursor;
}

bool ComicBookSettings::twoPagesMode() const
{
    return m_twopages;
}

bool ComicBookSettings::japaneseMode() const
{
    return m_japanese;
}

bool ComicBookSettings::continuousScrolling() const
{
    return m_contscroll;
}

ViewType ComicBookSettings::viewType() const
{
	return m_viewtype;
}

bool ComicBookSettings::scrollbarsVisible() const
{
    return m_scrollbars;
}

void ComicBookSettings::restoreGeometry(ComicMainWindow *w) const
{
    w->restoreGeometry(m_cfg->value(GRP_WINDOW OPT_GEOMETRY).toByteArray());
}

Size ComicBookSettings::pageSize() const
{
    return m_pagesize;
}

bool ComicBookSettings::smoothScaling() const
{
    return m_smoothscaling;
}

QString ComicBookSettings::lastDir() const
{
    return m_lastdir;
}

const QStringList& ComicBookSettings::recentlyOpened() const
{
    return m_recent;
}

QColor ComicBookSettings::background() const
{
	return m_bgcolor;
}

int ComicBookSettings::cacheSize() const
{
    return m_cachesize;
}

bool ComicBookSettings::cacheAutoAdjust() const
{
    return m_cacheadjust;
}

bool ComicBookSettings::cacheThumbnails() const
{
    return m_cachethumbs;
}

int ComicBookSettings::thumbnailsAge() const
{
    return m_thumbsage;
}

bool ComicBookSettings::preloadPages() const
{
    return m_preload;
}

bool ComicBookSettings::confirmExit() const
{
	return m_confirmexit;
}

bool ComicBookSettings::autoInfo() const
{
    return m_autoinfo;
}

bool ComicBookSettings::fullScreenHideMenu() const
{
    return m_fscrhidemenu;
}

bool ComicBookSettings::fullScreenHideStatusbar() const
{
    return m_fscrhidestatus;
}

bool ComicBookSettings::fullScreenHideToolbar() const
{
    return m_fscrhidetoolbar;
}

bool ComicBookSettings::showStatusbar() const
{
    return m_statusbar;
}

const QFont& ComicBookSettings::infoFont() const
{
    return m_font;
}

bool ComicBookSettings::showSplash() const
{
    return m_showsplash;
}

void ComicBookSettings::restoreDockLayout(ComicMainWindow *w) const
{
    w->restoreState(m_cfg->value(GRP_WINDOW OPT_DOCKLAYOUT).toByteArray());
}

QString ComicBookSettings::tmpDir() const
{
    return m_tmpdir;
}

void ComicBookSettings::embedPageNumbers(bool f)
{
    if (f != m_embedpagenumbers)
    {
        m_cfg->setValue(GRP_VIEW OPT_EMBEDPAGENUMBERS, m_embedpagenumbers = f);
        emit displaySettingsChanged(OPT_EMBEDPAGENUMBERS);
    }
}

void ComicBookSettings::smallCursor(bool f)
{
    if (f != m_smallcursor)
    {
        m_cfg->setValue(GRP_VIEW OPT_SMALLCURSOR, m_smallcursor = f);
        emit displaySettingsChanged(OPT_SMALLCURSOR);
    }
}

void ComicBookSettings::twoPagesMode(bool f)
{
    if (f != m_twopages)
    {
        m_cfg->setValue(GRP_VIEW OPT_TWOPAGES, m_twopages = f);
        emit displaySettingsChanged(OPT_TWOPAGES);
    }
}

void ComicBookSettings::japaneseMode(bool f)
{
    if (f != m_japanese)
    {
        m_cfg->setValue(GRP_VIEW OPT_JAPANESEMODE, m_japanese = f);
        emit displaySettingsChanged(OPT_JAPANESEMODE);
    }
}

void ComicBookSettings::continuousScrolling(bool f)
{
    if (f != m_contscroll)
    {
        m_cfg->setValue(GRP_VIEW OPT_CONTSCROLL, m_contscroll = f);
    }
}

void ComicBookSettings::viewType(ViewType t)
{
	if (t != m_viewtype)
	{
		m_cfg->setValue(GRP_VIEW OPT_VIEWTYPE, convert(viewtype2string, m_viewtype = t));
	}
}

void ComicBookSettings::scrollbarsVisible(bool f)
{
    if (f != m_scrollbars)
    {
        m_cfg->setValue(GRP_VIEW OPT_SCROLLBARS, m_scrollbars = f);
    }
}

void ComicBookSettings::saveGeometry(ComicMainWindow *w)
{
    m_cfg->setValue(GRP_WINDOW OPT_GEOMETRY, w->saveGeometry());
}

void ComicBookSettings::pageSize(Size s)
{
    if (s != m_pagesize)
    {
        m_cfg->setValue(GRP_VIEW OPT_PAGESIZE, convert(size2string, m_pagesize = s));
    }
}

void ComicBookSettings::smoothScaling(bool s)
{
    if (s != m_smoothscaling)
    {
        m_cfg->setValue(GRP_VIEW OPT_SMOOTHSCALING, m_smoothscaling = s);
        emit displaySettingsChanged(OPT_SMOOTHSCALING);
    }
}

void ComicBookSettings::lastDir(const QString &d)
{
    if (m_lastdir != d)
    {
        m_cfg->setValue(GRP_RUNTIME OPT_LASTDIR, m_lastdir = d);
    }
}

void ComicBookSettings::recentlyOpened(const QStringList &hist)
{
    m_recent = hist;
    m_cfg->setValue(GRP_RUNTIME OPT_RECENT, m_recent);
}

void ComicBookSettings::background(const QColor &color)
{
    if (color != m_bgcolor)
    {
        m_bgcolor = color;
        m_cfg->setValue(GRP_VIEW OPT_BACKGROUND, m_bgcolor.name());
        emit displaySettingsChanged(OPT_BACKGROUND);
    }
}

void ComicBookSettings::cacheSize(int s)
{
    if (s != m_cachesize)
    {
        if (s < 1)
        {
            s = 1;
        }
        m_cfg->setValue(GRP_MISC OPT_CACHESIZE, m_cachesize = s);
    }
}

void ComicBookSettings::cacheAutoAdjust(bool f)
{
    if (f != m_cacheadjust)
    {
        m_cacheadjust = f;
        m_cfg->setValue(GRP_MISC OPT_CACHEADJUST, m_cacheadjust = f);
    }
}

void ComicBookSettings::cacheThumbnails(bool f)
{
    if (f != m_cachethumbs)
    {
        m_cfg->setValue(GRP_MISC OPT_CACHETHUMBS, m_cachethumbs = f);
    }
}

void ComicBookSettings::thumbnailsAge(int n)
{
    if (n != m_thumbsage)
    {
        m_cfg->setValue(GRP_MISC OPT_THUMBSAGE, m_thumbsage = n);
    }
}

void ComicBookSettings::preloadPages(bool f)
{
    if (f != m_preload)
    {
        m_cfg->setValue(GRP_MISC OPT_PRELOAD, m_preload = f);
    }
}

void ComicBookSettings::confirmExit(bool f)
{
    if (f != m_confirmexit)
    {
        m_cfg->setValue(GRP_MISC OPT_CONFIRMEXIT, m_confirmexit = f);
    }
}

void ComicBookSettings::autoInfo(bool f)
{
    if (f != m_autoinfo)
    {
        m_cfg->setValue(GRP_MISC OPT_AUTOINFO, m_autoinfo = f);
    }
}

void ComicBookSettings::fullScreenHideMenu(bool f)
{
    if (f != m_fscrhidemenu)
    {
        m_cfg->setValue(GRP_VIEW OPT_FULLSCREENHIDEMENU, m_fscrhidemenu = f);
    }
}

void ComicBookSettings::fullScreenHideStatusbar(bool f)
{
    if (f != m_fscrhidestatus)
    {
        m_cfg->setValue(GRP_VIEW OPT_FULLSCREENHIDESTATUS, m_fscrhidestatus = f);
    }
}

void ComicBookSettings::fullScreenHideToolbar(bool f)
{
    if (f != m_fscrhidetoolbar)
    {
        m_cfg->setValue(GRP_VIEW OPT_FULLSCREENHIDETOOLBAR, m_fscrhidetoolbar = f);
    }
}

void ComicBookSettings::showStatusbar(bool f)
{
    if (f != m_statusbar)
    {
        m_cfg->setValue(GRP_VIEW OPT_STATUSBAR, m_statusbar = f);
    }
}

void ComicBookSettings::infoFont(const QFont &s)
{
    if (s != m_font)
    {
        m_font = s;
        m_cfg->setValue(GRP_VIEW OPT_FONT, m_font.toString());
    }
}

void ComicBookSettings::saveDockLayout(ComicMainWindow *w)
{
    m_cfg->setValue(GRP_WINDOW OPT_DOCKLAYOUT, w->saveState());
}

void ComicBookSettings::showSplash(bool f)
{
    if (f != m_showsplash)
    {
        m_cfg->setValue(GRP_MISC OPT_SHOWSPLASH, m_showsplash = f);
    }
}

void ComicBookSettings::tmpDir(const QString &dir)
{
    if (dir != m_tmpdir)
    {
        m_cfg->setValue(GRP_MISC OPT_TMPDIR, m_tmpdir = dir);
    }
}

