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
#include <stdlib.h>

#define OPT_VIEW                     "/View"
#define OPT_TWOPAGES                 "/TwoPages"
#define OPT_SCROLLBARS               "/Scrollbars"
#define OPT_SCALING                  "/Scaling"
#define OPT_PAGESIZE                 "/PageSize"
#define OPT_BACKGROUND               "/Background"
#define OPT_FULLSCREENHIDEMENU       "/FullScreenHideMenu"
#define OPT_FULLSCREENHIDETOOLBAR    "/FullScreenHideToolbar"
#define OPT_FULLSCREENHIDESCROLLBARS "/FullScreenHideScrollbars"

#define OPT_NAVI        "/Navigation"
#define OPT_CONTSCROLL  "/ContinuousScroll"

#define OPT_WINDOW      "/Window"
#define OPT_X           "/X"
#define OPT_Y           "/Y"
#define OPT_WIDTH       "/Width"
#define OPT_HEIGHT      "/Height"
#define OPT_DOCKLAYOUT  "/DockLayout"

#define OPT_MISC        "/Misc"
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
	{ComicImageView::BestFit,   "bestfit"},
	{-1, QString::null}
};

const ComicBookSettings::int2qstring ComicBookSettings::scaling2string[] = {
	{ComicImageView::Smooth, "smooth"},
	{ComicImageView::Fast,   "fast"},
	{-1, QString::null}
};

ComicBookSettings::ComicBookSettings(): QObject()/*{{{*/
{
	cfg = new QSettings();
	char *login = getenv("HOME");
	if (login)
	{
		QString path = QString(login) + "/.qcomicbook";
		cfg->insertSearchPath(QSettings::Unix, path);
	}
	cfg->beginGroup("/QComicBook");
}/*}}}*/

ComicBookSettings::~ComicBookSettings()/*{{{*/
{
	cfg->endGroup();
	delete cfg;
}/*}}}*/

bool ComicBookSettings::checkDirs()/*{{{*/
{
	dirsok = false;
	if (char *login = getenv("HOME"))
	{
		bkpath = QString(login) + "/.qcomicbook";
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
	return false;
}/*}}}*/

const QString& ComicBookSettings::bookmarksDir()/*{{{*/
{
	return bkpath;
}/*}}}*/

const QString& ComicBookSettings::thumbnailsDir()/*{{{*/
{
	return thpath;
}/*}}}*/

const QString& ComicBookSettings::convert(ComicImageView::Size s)/*{{{*/
{
	for (int i=0; !size2string[i].str.isNull(); i++)
		if (size2string[i].v == s)
			return size2string[i].str;
	return size2string[0].str;
}/*}}}*/

ComicImageView::Size ComicBookSettings::convertToSize(const QString &s)/*{{{*/
{
	for (int i=0; !size2string[i].str.isNull(); i++)
		if (size2string[i].str == s)
			return static_cast<ComicImageView::Size>(size2string[i].v);
	return ComicImageView::Original;
}/*}}}*/

const QString& ComicBookSettings::convert(ComicImageView::Scaling s)/*{{{*/
{
	for (int i=0; !scaling2string[i].str.isNull(); i++)
		if (scaling2string[i].v == s)
			return scaling2string[i].str;
	return scaling2string[0].str;
}/*}}}*/

ComicImageView::Scaling ComicBookSettings::convertToScaling(const QString &s)/*{{{*/
{
	for (int i=0; !scaling2string[i].str.isNull(); i++)
		if (scaling2string[i].str == s)
			return static_cast<ComicImageView::Scaling>(scaling2string[i].v);
	return ComicImageView::Smooth;
}/*}}}*/

void ComicBookSettings::load()/*{{{*/
{
	QString str_scaling;
	
	cfg->beginGroup(OPT_WINDOW);
		x = cfg->readNumEntry(OPT_X, 0);
		y = cfg->readNumEntry(OPT_Y, 0);
		w = cfg->readNumEntry(OPT_WIDTH, 640);
		h = cfg->readNumEntry(OPT_HEIGHT, 400);
		docklayout = cfg->readEntry(OPT_DOCKLAYOUT, QString::null);
	cfg->endGroup();
	cfg->beginGroup(OPT_VIEW);
		twopages = cfg->readBoolEntry(OPT_TWOPAGES, false);
		scrollbars = cfg->readBoolEntry(OPT_SCROLLBARS, false);
		scaling = convertToScaling(cfg->readEntry(OPT_SCALING, size2string[0].str));
		pagesize = convertToSize(cfg->readEntry(OPT_PAGESIZE, size2string[0].str));
		bgcolor.setNamedColor(cfg->readEntry(OPT_BACKGROUND, "#000000"));
		fscrhidetoolbar = cfg->readBoolEntry(OPT_FULLSCREENHIDETOOLBAR, true);
		fscrhidemenu = cfg->readBoolEntry(OPT_FULLSCREENHIDEMENU, true);
		fscrhidescroll = cfg->readBoolEntry(OPT_FULLSCREENHIDESCROLLBARS, true);
	cfg->endGroup();
	cfg->beginGroup(OPT_NAVI);
		contscroll = cfg->readBoolEntry(OPT_CONTSCROLL, true);
	cfg->endGroup();
	cfg->beginGroup(OPT_MISC);
		lastdir = cfg->readEntry(OPT_LASTDIR, QString::null);
		recent = cfg->readListEntry(OPT_RECENT);
		cachesize = cfg->readNumEntry(OPT_CACHESIZE, 3);
		preload = cfg->readBoolEntry(OPT_PRELOAD, true);
		confirmexit = cfg->readBoolEntry(OPT_CONFIRMEXIT, true);
		autoinfo = cfg->readBoolEntry(OPT_AUTOINFO, false);
		thumbsage = cfg->readNumEntry(OPT_THUMBSAGE, 7);
		cachethumbs = cfg->readBoolEntry(OPT_CACHETHUMBS, true);
	cfg->endGroup();
}/*}}}*/

bool ComicBookSettings::getTwoPagesMode() const/*{{{*/
{
	return twopages;
}/*}}}*/

bool ComicBookSettings::getContinuousScrolling() const/*{{{*/
{
	return contscroll;
}/*}}}*/

bool ComicBookSettings::getScrollbarsVisible() const/*{{{*/
{
	return scrollbars;
}/*}}}*/

QRect ComicBookSettings::getGeometry() const/*{{{*/
{
	return QRect(x, y, w, h);
}/*}}}*/

ComicImageView::Size ComicBookSettings::getPageSize() const/*{{{*/
{
	return pagesize;
}/*}}}*/

ComicImageView::Scaling ComicBookSettings::getScaling() const/*{{{*/
{
	return scaling;
}/*}}}*/

QString ComicBookSettings::getLastDir() const/*{{{*/
{
	return lastdir;
}/*}}}*/

const History& ComicBookSettings::getRecentlyOpened() const/*{{{*/
{
	return recent;
}/*}}}*/

QColor ComicBookSettings::getBackground() const/*{{{*/
{
	return bgcolor;
}/*}}}*/

int ComicBookSettings::getCacheSize() const/*{{{*/
{
	return cachesize;
}/*}}}*/

bool ComicBookSettings::getCacheThumbnails() const/*{{{*/
{
	return cachethumbs;
}/*}}}*/

int ComicBookSettings::getThumbnailsAge() const/*{{{*/
{
	return thumbsage;
}/*}}}*/

bool ComicBookSettings::getPreload() const/*{{{*/
{
	return preload;
}/*}}}*/

bool ComicBookSettings::getConfirmExit() const/*{{{*/
{
	return confirmexit;
}/*}}}*/

bool ComicBookSettings::getAutoInfo() const/*{{{*/
{
	return autoinfo;
}/*}}}*/

bool ComicBookSettings::getFullScreenHideToolbar() const/*{{{*/
{
	return fscrhidetoolbar;
}/*}}}*/

bool ComicBookSettings::getFullScreenHideMenu() const/*{{{*/
{
	return fscrhidemenu;
}/*}}}*/

bool ComicBookSettings::getFullScreenHideScrollbars() const/*{{{*/
{
	return fscrhidescroll;
}/*}}}*/

void ComicBookSettings::restoreDockLayout(QMainWindow *w)/*{{{*/
{
	QTextStream str(&docklayout, IO_ReadOnly);
	str >> *w;
}/*}}}*/

void ComicBookSettings::setTwoPagesMode(bool f)/*{{{*/
{
	if (f != twopages)
		cfg->writeEntry(OPT_VIEW OPT_TWOPAGES, twopages = f);
}/*}}}*/

void ComicBookSettings::setContinuousScrolling(bool f)/*{{{*/
{
	if (f != contscroll)
		cfg->writeEntry(OPT_NAVI OPT_CONTSCROLL, contscroll = f);
}/*}}}*/

void ComicBookSettings::setScrollbarsVisible(bool f)/*{{{*/
{
	if (f != scrollbars)
		cfg->writeEntry(OPT_VIEW OPT_SCROLLBARS, scrollbars = f);
}/*}}}*/

void ComicBookSettings::setGeometry(const QRect g)/*{{{*/
{
	if (x != g.x() || y != g.y() || w != g.width() || h != g.height())
	{
		cfg->beginGroup(OPT_WINDOW);
		cfg->writeEntry(OPT_X, x = g.x());
		cfg->writeEntry(OPT_Y, y = g.y());
		cfg->writeEntry(OPT_WIDTH, w = g.width());
		cfg->writeEntry(OPT_HEIGHT, h = g.height());
		cfg->endGroup();
	}
}/*}}}*/

void ComicBookSettings::setPageSize(ComicImageView::Size s)/*{{{*/
{
	if (s != pagesize)
		cfg->writeEntry(OPT_VIEW OPT_PAGESIZE, convert(pagesize = s));
}/*}}}*/

void ComicBookSettings::setScaling(ComicImageView::Scaling s)/*{{{*/
{
	if (s != scaling)
	{
		cfg->writeEntry(OPT_VIEW OPT_SCALING, convert(scaling = s));
		emit scalingMethodChanged(scaling);
	}
}/*}}}*/

void ComicBookSettings::setLastDir(const QString &d)/*{{{*/
{
	if (lastdir != d)
		cfg->writeEntry(OPT_MISC OPT_LASTDIR, lastdir = d);
}/*}}}*/

void ComicBookSettings::setRecentlyOpened(const History &hist)/*{{{*/
{
	recent = hist;
	cfg->writeEntry(OPT_MISC OPT_RECENT, recent.getAll());
}/*}}}*/

void ComicBookSettings::setBackground(const QColor &color)/*{{{*/
{
	if (color != bgcolor)
	{
		bgcolor = color;
		cfg->writeEntry(OPT_VIEW OPT_BACKGROUND, bgcolor.name());
		emit backgroundChanged(bgcolor);
	}
}/*}}}*/

void ComicBookSettings::setCacheSize(int s)/*{{{*/
{
	if (s != cachesize)
		cfg->writeEntry(OPT_MISC OPT_CACHESIZE, cachesize = s);
}/*}}}*/

void ComicBookSettings::setCacheThumbnails(bool f)/*{{{*/
{
	if (f != cachethumbs)
		cfg->writeEntry(OPT_MISC OPT_CACHETHUMBS, cachethumbs = f);
}/*}}}*/

void ComicBookSettings::setThumbnailsAge(int n)/*{{{*/
{
	if (n != thumbsage)
		cfg->writeEntry(OPT_MISC OPT_THUMBSAGE, thumbsage = n);
}/*}}}*/

void ComicBookSettings::setPreload(bool f)/*{{{*/
{
	if (f != preload)
		cfg->writeEntry(OPT_MISC OPT_PRELOAD, preload = f);
}/*}}}*/

void ComicBookSettings::setConfirmExit(bool f)/*{{{*/
{
	if (f != confirmexit)
		cfg->writeEntry(OPT_MISC OPT_CONFIRMEXIT, confirmexit = f);
}/*}}}*/

void ComicBookSettings::setAutoInfo(bool f)/*{{{*/
{
	if (f != autoinfo)
		cfg->writeEntry(OPT_MISC OPT_AUTOINFO, autoinfo = f);
}/*}}}*/

void ComicBookSettings::setFullScreenHideToolbar(bool f)/*{{{*/
{
	if (f != fscrhidetoolbar)
		cfg->writeEntry(OPT_VIEW OPT_FULLSCREENHIDETOOLBAR, fscrhidetoolbar = f);
}/*}}}*/

void ComicBookSettings::setFullScreenHideMenu(bool f)/*{{{*/
{
	if (f != fscrhidemenu)
		cfg->writeEntry(OPT_VIEW OPT_FULLSCREENHIDEMENU, fscrhidemenu = f);
}/*}}}*/

void ComicBookSettings::setFullScreenHideScrollbars(bool f)/*{{{*/
{
	if (f != fscrhidescroll)
		cfg->writeEntry(OPT_VIEW OPT_FULLSCREENHIDESCROLLBARS, fscrhidescroll = f);
}/*}}}*/

void ComicBookSettings::saveDockLayout(QMainWindow *w)/*{{{*/
{
	QString tmp;
	QTextStream str(&tmp, IO_WriteOnly);
	str << *w;
	cfg->writeEntry(OPT_WINDOW OPT_DOCKLAYOUT, tmp);
}/*}}}*/

