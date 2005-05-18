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

#include "config.h"
#include "bookmarks.h"
#include "comicmain.h"
#include "icons.h"
#include "cbicons.h"
#include "cbinfo.h"
#include "imgview.h"
#include "imgarchivesink.h"
#include "imgsinkfactory.h"
#include "aboutdialog.h"
#include "cbsettings.h"
#include "history.h"
#include "helpbrowser.h"
#include "cbconfigdialog.h"
#include "statusbar.h"
#include "thumbnailswin.h"
#include "thumbnailsview.h"
#include "thumbnailloader.h"
#include "bookmarkmanager.h"
#include <qimage.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qstringlist.h>
#include <qaction.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qtoolbar.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qframe.h>
#include <jumptopagewin.h>
#include <qprogressdialog.h>
#include <typeinfo>

//
// archives extensions used for Open File dialog and filtering out files in OpenNext() function
const QString ComicMainWindow::ARCH_EXTENSIONS = "*.rar *.cbr *.zip *.cbz *.ace *.cba *.tar.gz *.tgz *.cbg *.tar.bz2 *.cbb";

ComicMainWindow::ComicMainWindow(QWidget *parent): QMainWindow(parent, NULL, WType_TopLevel|WDestructiveClose), sink(NULL), currpage(0)/*{{{*/
{
	updateCaption();
	setIcon(Icons::get(ICON_APPICON).pixmap(QIconSet::Small, true));
	setMinimumSize(320, 200);

	recentfiles = new History(10);
	cfg = &ComicBookSettings::instance();

	setGeometry(cfg->geometry());

	//
	// statusbar
	statusbar = new StatusBar(this);
	toggleStatusbarAction = new QAction(tr("Statusbar"), QKeySequence(), this);
	toggleStatusbarAction->setToggleAction(true);
	connect(toggleStatusbarAction, SIGNAL(toggled(bool)), statusbar, SLOT(setShown(bool)));
	toggleStatusbarAction->setOn(cfg->showStatusbar());
	statusbar->setShown(cfg->showStatusbar());
	
	//
	// comic view
	view = new ComicImageView(this, cfg->pageSize(), cfg->pageScaling(), ComicImageView::Right, cfg->background());
	setCentralWidget(view);
	view->setFocus();
	view->setSmallCursor(cfg->smallCursor());
	connect(cfg, SIGNAL(backgroundChanged(const QColor&)), view, SLOT(setBackground(const QColor&)));
	connect(cfg, SIGNAL(scalingMethodChanged(ComicImageView::Scaling)), view, SLOT(setScaling(ComicImageView::Scaling)));
	connect(cfg, SIGNAL(cursorChanged(bool)), view, SLOT(setSmallCursor(bool)));

	//
	// thumbnails view
	thumbswin = new ThumbnailsWindow(QDockWindow::InDock, this);
	moveDockWindow(thumbswin, Qt::DockLeft); //initial position of thumbnails window
	toggleThumbnailsAction = new QAction(Icons::get(ICON_THUMBNAILS), tr("Thumbnails"), ALT+Key_T, this);
	toggleThumbnailsAction->setToggleAction(true);
	connect(thumbswin, SIGNAL(requestedPage(int, bool)), this, SLOT(jumpToPage(int, bool)));
	connect(thumbswin, SIGNAL(visibilityChanged(bool)), this, SLOT(thumbnailsVisibilityChanged(bool)));
	connect(thumbswin, SIGNAL(visibilityChanged(bool)), toggleThumbnailsAction, SLOT(setOn(bool)));
	connect(toggleThumbnailsAction, SIGNAL(toggled(bool)), thumbswin, SLOT(setShown(bool)));
	
	//
	// global keyboard accelerators
	QAction *openArchiveAction = new QAction(tr("Open archive"), CTRL+Key_O, this);
	connect(openArchiveAction, SIGNAL(activated()), this, SLOT(browseArchive()));
	QAction *openDirAction = new QAction(tr("Open directory"), CTRL+Key_D, this);
	connect(openDirAction, SIGNAL(activated()), this, SLOT(browseDirectory()));
	openNextAction = new QAction(tr("Open next"), CTRL+Key_N, this);
	connect(openNextAction, SIGNAL(activated()), this, SLOT(openNext()));
	openPrevAction = new QAction(tr("Open previous"), CTRL+Key_P, this);
	connect(openPrevAction, SIGNAL(activated()), this, SLOT(openPrevious()));
	QAction *fullScreenAction = new QAction(tr("&Fullscreen"), Key_F11, this);
	connect(fullScreenAction, SIGNAL(activated()), this, SLOT(toggleFullScreen()));
	nextPageAction = new QAction(Icons::get(ICON_NEXTPAGE), tr("Next page"), Key_PageDown, this);
	connect(nextPageAction, SIGNAL(activated()), this, SLOT(nextPage()));
	forwardPageAction = new QAction(Icons::get(ICON_FORWARD), tr("5 pages forward"), QKeySequence(), this);
	connect(forwardPageAction, SIGNAL(activated()), this, SLOT(forwardPages()));
	backwardPageAction = new QAction(Icons::get (ICON_BACKWARD), tr("5 pages backward"), QKeySequence(), this);
	connect(backwardPageAction, SIGNAL(activated()), this, SLOT(backwardPages()));
	
	QAction *jumpDownAction = new QAction(QString::null, Key_Space, this);
	connect(jumpDownAction, SIGNAL(activated()), view, SLOT(jumpDown()));
	QAction *jumpUpAction = new QAction(QString::null, Key_Backspace, this);
	connect(jumpUpAction, SIGNAL(activated()), view, SLOT(jumpUp()));

	prevPageAction = new QAction(Icons::get(ICON_PREVPAGE), tr("&Previous page"), Key_PageUp, this);
	connect(prevPageAction, SIGNAL(activated()), this, SLOT(prevPage()));

	pageTopAction = new QAction(Icons::get(ICON_PAGETOP), tr("Page top"), Key_Home, this);
	connect(pageTopAction, SIGNAL(activated()), view, SLOT(scrollToTop()));
	pageBottomAction = new QAction(Icons::get(ICON_PAGEBOTTOM), tr("Page bottom"), Key_End, this);
	connect(pageBottomAction, SIGNAL(activated()), view, SLOT(scrollToBottom()));
	QAction *scrollRightAction = new QAction(tr("Scroll right"), Key_Right, this);
	connect(scrollRightAction, SIGNAL(activated()), view, SLOT(scrollRight()));
	QAction *scrollLeftAction = new QAction(tr("Scroll left"), Key_Left, this);
	connect(scrollLeftAction, SIGNAL(activated()), view, SLOT(scrollLeft()));
	QAction *scrollRightFastAction = new QAction(tr("Fast scroll right"), SHIFT+Key_Right, this);
	connect(scrollRightFastAction, SIGNAL(activated()), view, SLOT(scrollRightFast()));
	QAction *scrollLeftFastAction = new QAction(tr("Fast scroll left"), SHIFT+Key_Left, this);
	connect(scrollLeftFastAction, SIGNAL(activated()), view, SLOT(scrollLeftFast()));
	QAction *scrollUpAction = new QAction(tr("Scroll up"), Key_Up, this);
	connect(scrollUpAction, SIGNAL(activated()), view, SLOT(scrollUp()));
	QAction *scrollDownAction = new QAction(tr("Scroll down"), Key_Down, this);
	connect(scrollDownAction, SIGNAL(activated()), view, SLOT(scrollDown()));
	QAction *scrollUpFastAction = new QAction(tr("Fast scroll up"), SHIFT+Key_Up, this);
	connect(scrollUpFastAction, SIGNAL(activated()), view, SLOT(scrollUpFast()));
	QAction *scrollDownFastAction = new QAction(tr("Fast scroll down"), SHIFT+Key_Down, this);
	connect(scrollDownFastAction, SIGNAL(activated()), view, SLOT(scrollDownFast()));

	QActionGroup *scaleActions = new QActionGroup(this);
	QAction *fitWidthAction = new QAction(Icons::get(ICON_FITWIDTH), tr("Fit width"), ALT+Key_W, scaleActions);
	fitWidthAction->setToggleAction(true);
	connect(fitWidthAction, SIGNAL(activated()), view, SLOT(setSizeFitWidth()));
	QAction *fitHeightAction = new QAction(Icons::get(ICON_FITHEIGHT), tr("Fit height"), ALT+Key_H, scaleActions);
	fitHeightAction->setToggleAction(true);
	connect(fitHeightAction, SIGNAL(activated()), view, SLOT(setSizeFitHeight()));
	QAction *wholePageAction = new QAction(Icons::get(ICON_WHOLEPAGE), tr("Whole page"), ALT+Key_A, scaleActions);
	wholePageAction->setToggleAction(true);
	connect(wholePageAction, SIGNAL(activated()), view, SLOT(setSizeWholePage()));
	QAction *originalSizeAction = new QAction(Icons::get(ICON_ORGSIZE), tr("Original size"), ALT+Key_O, scaleActions);
	originalSizeAction->setToggleAction(true);
	connect(originalSizeAction, SIGNAL(activated()), view, SLOT(setSizeOriginal()));
	QAction *bestFitAction = new QAction(Icons::get(ICON_BESTFIT), tr("Best fit"), ALT+Key_B, scaleActions);
	bestFitAction->setToggleAction(true);
	connect(bestFitAction, SIGNAL(activated()), view, SLOT(setSizeBestFit()));
	mangaModeAction = new QAction(Icons::get(ICON_JAPANESE), tr("Japanese mode"), CTRL+Key_J, this);
	mangaModeAction->setToggleAction(true);
	connect(mangaModeAction, SIGNAL(toggled(bool)), this, SLOT(toggleJapaneseMode(bool)));
	twoPagesAction = new QAction(Icons::get(ICON_TWOPAGES), tr("Two pages"), CTRL+Key_T, this);
	twoPagesAction->setToggleAction(true);
	connect(twoPagesAction, SIGNAL(toggled(bool)), this, SLOT(toggleTwoPages(bool)));
	
	showInfoAction = new QAction(Icons::get(ICON_INFO), tr("Info"), ALT+Key_I, this);
	connect(showInfoAction, SIGNAL(activated()), this, SLOT(showInfo()));
	QAction *exitFullscreenAction = new QAction(QString::null, Key_Escape, this);
	connect(exitFullscreenAction, SIGNAL(activated()), this, SLOT(exitFullscreen()));
		
	QAction *which = originalSizeAction;
	switch (cfg->pageSize())
	{
		case ComicImageView::FitWidth:  which = fitWidthAction; break;
		case ComicImageView::FitHeight: which = fitHeightAction; break;
		case ComicImageView::BestFit:   which = bestFitAction; break;
		case ComicImageView::WholePage: which = wholePageAction; break;
		case ComicImageView::Original:  which = originalSizeAction; break;
	}
	which->setOn(true);
	
	//
	// Toolbar
	toolbar = new QToolBar(tr("Toolbar"), this);
	toggleToolbarAction = new QAction(tr("Toolbar"), QKeySequence(), this);
	toggleToolbarAction->setToggleAction(true);
	showInfoAction->addTo(toolbar);
	toggleThumbnailsAction->addTo(toolbar);
	toolbar->addSeparator();
	twoPagesAction->addTo(toolbar);
	mangaModeAction->addTo(toolbar);
	toolbar->addSeparator();
	prevPageAction->addTo(toolbar);
	nextPageAction->addTo(toolbar);
	backwardPageAction->addTo(toolbar);
	forwardPageAction->addTo(toolbar);
	pageTopAction->addTo(toolbar);
	pageBottomAction->addTo(toolbar);
	toolbar->addSeparator();
	originalSizeAction->addTo(toolbar);
	fitWidthAction->addTo(toolbar);
	fitHeightAction->addTo(toolbar);
	wholePageAction->addTo(toolbar);
	bestFitAction->addTo(toolbar);
	connect(toggleToolbarAction, SIGNAL(toggled(bool)), toolbar, SLOT(setShown(bool)));
	connect(toolbar, SIGNAL(visibilityChanged(bool)), this, SLOT(toolbarVisibilityChanged(bool)));

	//
	// Context menu
	pageinfo = new QLabel(view->contextMenu());
	pageinfo->setMargin(3);
	pageinfo->setAlignment(Qt::AlignHCenter);
	pageinfo->setFrameStyle(QFrame::Box | QFrame::Raised);
	nextPageAction->addTo(view->contextMenu());
	prevPageAction->addTo(view->contextMenu());
	view->contextMenu()->insertSeparator();
	fitWidthAction->addTo(view->contextMenu());
	fitHeightAction->addTo(view->contextMenu());
	wholePageAction->addTo(view->contextMenu());
	originalSizeAction->addTo(view->contextMenu());
	bestFitAction->addTo(view->contextMenu());
	view->contextMenu()->insertSeparator();
	twoPagesAction->addTo(view->contextMenu());
	mangaModeAction->addTo(view->contextMenu());
	view->contextMenu()->insertSeparator();
	fullScreenAction->addTo(view->contextMenu());
	view->contextMenu()->insertSeparator();
	view->contextMenu()->insertItem(pageinfo);
	
	//
	// File menu
	file_menu = new QPopupMenu(this);
	openDirAction->addTo(file_menu);
	openArchiveAction->addTo(file_menu);
	openNextAction->addTo(file_menu);
	openPrevAction->addTo(file_menu);
	recent_menu = new QPopupMenu(this);
	file_menu->insertItem(tr("Recently opened"), recent_menu);
	connect(recent_menu, SIGNAL(activated(int)), this, SLOT(recentSelected(int)));
	showInfoAction->addTo(file_menu);
	file_menu->insertItem(tr("Settings"), this, SLOT(showConfigDialog()));
	file_menu->insertSeparator();
	close_id = file_menu->insertItem(tr("Close"), this, SLOT(closeSink()));
	file_menu->insertSeparator();
	file_menu->insertItem(tr("&Quit"), this, SLOT(close()));
	menuBar()->insertItem(tr("&File"), file_menu);
	
	//
	// View menu
	bool f;
	view_menu = new QPopupMenu(this);
	view_menu->setCheckable(true);
	originalSizeAction->addTo(view_menu);
	fitWidthAction->addTo(view_menu);
	fitHeightAction->addTo(view_menu);
	wholePageAction->addTo(view_menu);
	bestFitAction->addTo(view_menu);
	view_menu->insertSeparator();
	fullScreenAction->addTo(view_menu);
	view_menu->insertSeparator();
	twoPagesAction->addTo(view_menu);
	mangaModeAction->addTo(view_menu);
	toggleThumbnailsAction->addTo(view_menu);
	view_menu->insertSeparator();
	scrv_id = view_menu->insertItem(tr("Scrollbars"), this, SLOT(toggleScrollbars()));
	view_menu->setItemChecked(scrv_id, f = cfg->scrollbarsVisible());
	toggleToolbarAction->addTo(view_menu);
	toggleStatusbarAction->addTo(view_menu);
	menuBar()->insertItem(tr("&View"), view_menu);
	view->enableScrollbars(cfg->scrollbarsVisible());
	
	//
	// Navigation menu
	navi_menu = new QPopupMenu(this);
	nextPageAction->addTo(navi_menu);
	prevPageAction->addTo(navi_menu);
	navi_menu->insertSeparator();
	forwardPageAction->addTo(navi_menu);
	backwardPageAction->addTo(navi_menu);
	navi_menu->insertSeparator();
	jumpto_id = navi_menu->insertItem(tr("Jump to page..."), this, SLOT(showJumpToPage()));
	firstpage_id = navi_menu->insertItem(tr("First page"), this, SLOT(firstPage()));
	lastpage_id = navi_menu->insertItem(tr("Last page"), this, SLOT(lastPage()));
	navi_menu->insertSeparator();
	pageTopAction->addTo(navi_menu);
	pageBottomAction->addTo(navi_menu);
	navi_menu->insertSeparator();
	contscr_id = navi_menu->insertItem(tr("Continuous scrolling"), this, SLOT(toggleContinousScroll()));

	twoPagesAction->setOn(cfg->twoPagesMode());
	mangaModeAction->setOn(cfg->japaneseMode());
	navi_menu->setItemChecked(contscr_id, cfg->continuousScrolling());

	if (cfg->continuousScrolling())
	{
		connect(view, SIGNAL(bottomReached()), this, SLOT(nextPage()));
		connect(view, SIGNAL(topReached()), this, SLOT(prevPageBottom()));
	}
	menuBar()->insertItem(tr("&Navigation"), navi_menu);
	
	//
	// Bookmarks menu
	bookmarks_menu = new QPopupMenu(this);
	bookmarks = new Bookmarks(bookmarks_menu);
	menuBar()->insertItem(tr("&Bookmarks"), bookmarks_menu);
	setbookmark_id = bookmarks_menu->insertItem(tr("Set bookmark for this comicbook"), this, SLOT(setBookmark()));
	rmvbookmark_id = bookmarks_menu->insertItem(tr("Remove bookmark for this comicbook"), this, SLOT(removeBookmark()));
	bookmarks_menu->insertItem(tr("Manage bookmarks"), this, SLOT(openBookmarksManager()));
	bookmarks_menu->insertSeparator();
	bookmarks->load();
	connect(bookmarks_menu, SIGNAL(activated(int)), this, SLOT(bookmarkSelected(int)));
	
	//
	// Help menu
	QPopupMenu *help_menu = new QPopupMenu(this);
	help_menu->insertItem(tr("Index"), this, SLOT(showHelp()));
	help_menu->insertItem(tr("About"), this, SLOT(showAbout()));
	menuBar()->insertItem(tr("&Help"), help_menu);
	
	lastdir = cfg->lastDir();
	*recentfiles = cfg->recentlyOpened();
	setRecentFilesMenu(*recentfiles);

	cfg->restoreDockLayout(this);

	enableComicBookActions(false);
}/*}}}*/

ComicMainWindow::~ComicMainWindow()/*{{{*/
{
	if (cfg->cacheThumbnails())
		ImgDirSink::removeThumbnails(cfg->thumbnailsAge());

	cfg->geometry(frameGeometry());
	cfg->saveDockLayout(this);
	cfg->scrollbarsVisible(view_menu->isItemChecked(scrv_id));
	cfg->twoPagesMode(twoPagesAction->isOn());
	cfg->japaneseMode(mangaModeAction->isOn());
	cfg->continuousScrolling(navi_menu->isItemChecked(contscr_id));
	cfg->lastDir(lastdir);
	cfg->recentlyOpened(*recentfiles);
	cfg->pageSize(view->getSize());
	cfg->showStatusbar(toggleStatusbarAction->isOn());
	
	bookmarks->save();

	delete recentfiles;
	delete bookmarks;

	if (sink)
		delete sink;
}/*}}}*/

void ComicMainWindow::enableComicBookActions(bool f)/*{{{*/
{
	//
	// file menu
	const bool x = f && sink && typeid(*sink) == typeid(ImgArchiveSink);
	file_menu->setItemEnabled(close_id, f);
	showInfoAction->setEnabled(f);
	openNextAction->setEnabled(x);
	openPrevAction->setEnabled(x);
	
	//
	// navigation menu
	navi_menu->setItemEnabled(firstpage_id, f);
	navi_menu->setItemEnabled(lastpage_id, f);
	navi_menu->setItemEnabled(jumpto_id, f);
	nextPageAction->setEnabled(f);
	prevPageAction->setEnabled(f);
	backwardPageAction->setEnabled(f);
	forwardPageAction->setEnabled(f);
	pageTopAction->setEnabled(f);
	pageBottomAction->setEnabled(f);
	
	//
	// bookmarks menu
	bookmarks_menu->setItemEnabled(setbookmark_id, f);
	bookmarks_menu->setItemEnabled(rmvbookmark_id, f);
}/*}}}*/

void ComicMainWindow::keyPressEvent(QKeyEvent *e)/*{{{*/
{
	if ((e->key()>=Qt::Key_1) && (e->key()<=Qt::Key_9))
	{
		e->accept();
		showJumpToPage(e->text());
	}
	else
		QMainWindow::keyPressEvent(e);
}/*}}}*/

void ComicMainWindow::closeEvent(QCloseEvent *e)/*{{{*/
{
	return (!cfg->confirmExit() || confirmExit()) ? e->accept() : e->ignore();
}/*}}}*/

bool ComicMainWindow::confirmExit()/*{{{*/
{
	return QMessageBox::question(this, "Leave QComicBook?", "Do you really want to quit QComicBook?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
}/*}}}*/

void ComicMainWindow::thumbnailsVisibilityChanged(bool f)/*{{{*/
{
	if (f && sink)
	{
		int max = sink->numOfImages();
		for (int i=0; i<max; i++)
			if (!thumbswin->view()->isLoaded(i))
				sink->requestThumbnail(i);
	}
}/*}}}*/

void ComicMainWindow::toolbarVisibilityChanged(bool f)/*{{{*/
{
	toggleToolbarAction->setOn(f);
}/*}}}*/

void ComicMainWindow::toggleScrollbars()/*{{{*/
{
	bool f = view_menu->isItemChecked(scrv_id);
	view_menu->setItemChecked(scrv_id, !f);
	view->enableScrollbars(!f);
}/*}}}*/

void ComicMainWindow::toggleContinousScroll()/*{{{*/
{
	bool f = navi_menu->isItemChecked(contscr_id);
	navi_menu->setItemChecked(contscr_id, !f);
	if (f)
	{
		view->disconnect(SIGNAL(bottomReached()), this);
		view->disconnect(SIGNAL(topReached()), this);
	}
	else
	{	connect(view, SIGNAL(bottomReached()), this, SLOT(nextPage()));
		connect(view, SIGNAL(topReached()), this, SLOT(prevPageBottom()));
	}
}/*}}}*/

void ComicMainWindow::toggleTwoPages(bool f)/*{{{*/
{
	twoPagesAction->setOn(f);
	jumpToPage(currpage, true);
}/*}}}*/

void ComicMainWindow::toggleJapaneseMode(bool f)/*{{{*/
{
	mangaModeAction->setOn(f);
	if (twoPagesAction->isOn())
		jumpToPage(currpage, true);
}/*}}}*/

void ComicMainWindow::updateCaption()/*{{{*/
{
	QString c = "QComicBook";
	if (sink)
		c += " - " + sink->getName();
	setCaption(c);
}/*}}}*/

void ComicMainWindow::setRecentFilesMenu(const History &hist)/*{{{*/
{
	QStringList list = hist.getAll();
	recent_menu->clear();
	for (QStringList::const_iterator it = list.begin(); it != list.end(); it++)
		recent_menu->insertItem(*it);
}/*}}}*/

void ComicMainWindow::recentSelected(int id)/*{{{*/
{
	const QString &fname = recent_menu->text(id);
	if (fname != QString::null)
	{
		QFileInfo finfo(fname);
		if (!finfo.exists())
		{
			recentfiles->remove(fname);
			recent_menu->removeItem(id);
		}
		open(fname, 0);
	}
}/*}}}*/

void ComicMainWindow::sinkReady(const QString &path)/*{{{*/
{
	recentfiles->append(path);
	setRecentFilesMenu(*recentfiles);

	enableComicBookActions(true);
	updateCaption();
	statusbar->setName(sink->getFullName());

	thumbswin->view()->setPages(sink->numOfImages());

	//
	// request thumbnails for all pages
	if (thumbswin->isVisible())
		sink->requestThumbnails(0, sink->numOfImages());

	jumpToPage(currpage, true);
	if (cfg->autoInfo())
		showInfo();
}/*}}}*/

void ComicMainWindow::sinkError(int code)/*{{{*/
{
	QString msg;

	switch (code)
	{
		case SINKERR_EMPTY: msg = tr("no images found"); break;
		case SINKERR_UNKNOWNFILE: msg = tr("unknown archive"); break;
		case SINKERR_ACCESS: msg = tr("can't access directory"); break;
		case SINKERR_NOTFOUND: msg = tr("file/directory not found"); break;
		case SINKERR_NOTSUPPORTED: msg = tr("archive not supported"); break;
		case SINKERR_ARCHEXIT: msg = tr("archive extractor exited with error"); break;
		default: break;
	}
	QMessageBox::critical(this, "QComicBook error", "Error opening comicbook: " + msg, 
			QMessageBox::Ok, QMessageBox::NoButton);
	closeSink();
}/*}}}*/

void ComicMainWindow::browseDirectory()/*{{{*/
{
	const QString dir = QFileDialog::getExistingDirectory(lastdir, this, 
			NULL, tr("Choose a directory") );
	if (!dir.isEmpty())
		open(dir, 0);
}/*}}}*/

void ComicMainWindow::browseArchive()/*{{{*/
{
	const QString file = QFileDialog::getOpenFileName(lastdir,
			"Archives (" + ARCH_EXTENSIONS + ");;All files (*)",
			this, NULL, tr("Choose a file") );
	if (!file.isEmpty())
		open(file, 0);
}/*}}}*/

void ComicMainWindow::open(const QString &path, int page)/*{{{*/
{
	const QFileInfo f(path);
	const QString fullname = f.absFilePath();

	if (sink && sink->getFullName() == fullname) //trying to open same dir?
		return;

	lastdir = f.dirPath(true);
	currpage = page;

	closeSink();

	sink = ImgSinkFactory::instance().createImgSink(path);
	sink->thumbnailLoader().setReciever(thumbswin);
	sink->thumbnailLoader().setUseCache(cfg->cacheThumbnails());

	connect(sink, SIGNAL(sinkReady(const QString&)), this, SLOT(sinkReady(const QString&)));
	connect(sink, SIGNAL(sinkError(int)), this, SLOT(sinkError(int)));
	
	QProgressDialog *win = new QProgressDialog(tr("Please wait. Opening comicbook"), 0, 1, this, 0, true, WDestructiveClose);
	win->setCaption(caption());
	win->setAutoClose(true);
	win->setAutoReset(false);
	
	connect(sink, SIGNAL(progress(int, int)), win, SLOT(setProgress(int, int)));
	connect(sink, SIGNAL(sinkReady(const QString&)), win, SLOT(close()));
	connect(sink, SIGNAL(sinkError(int)), win, SLOT(close()));

	win->show();
	
	sink->open(fullname);
}/*}}}*/

void ComicMainWindow::openNext()/*{{{*/
{
	if (ImgArchiveSink *archive = dynamic_cast<ImgArchiveSink *>(sink))
	{
		QFileInfo finfo(sink->getFullName());
		QDir dir(finfo.dirPath(true)); //get the full path of current cb
		QStringList files = dir.entryList(ARCH_EXTENSIONS, QDir::Files|QDir::Readable, QDir::Name);
		QStringList::iterator it = files.find(finfo.fileName()); //find current cb
		if (it != files.end())
			if (++it != files.end()) //get next file name
			{
				currpage = 0;
				open(dir.filePath(*it, true), 0);
			}
	}
}/*}}}*/

void ComicMainWindow::openPrevious()/*{{{*/
{
	if (ImgArchiveSink *archive = dynamic_cast<ImgArchiveSink *>(sink))
	{
		QFileInfo finfo(sink->getFullName());
		QDir dir(finfo.dirPath(true)); //get the full path of current cb
		QStringList files = dir.entryList(ARCH_EXTENSIONS, QDir::Files|QDir::Readable, QDir::Name);
		QStringList::iterator it = files.find(finfo.fileName()); //find current cb
		if (it != files.end() && it != files.begin())
		{
			currpage = 0;
			open(dir.filePath(*(--it), true), 0);
		}
	}
}/*}}}*/

void ComicMainWindow::toggleFullScreen()/*{{{*/
{
	if (isFullScreen())
	{
		exitFullscreen();
	}
	else
	{
		if (cfg->fullScreenHideMenu())
			menuBar()->hide();
		if (cfg->fullScreenHideStatusbar())
			statusbar->hide();
		showFullScreen();
	}
}/*}}}*/

void ComicMainWindow::exitFullscreen()/*{{{*/
{
	if (isFullScreen())
	{
		menuBar()->show();
		if (toggleStatusbarAction->isOn())
			statusbar->show();
		showNormal();
	}
}/*}}}*/

void ComicMainWindow::nextPage()/*{{{*/
{
	if (sink)
	{
		if (twoPagesAction->isOn() && cfg->twoPagesStep())
		{
			if (currpage < sink->numOfImages() - 2) //do not change pages if last two pages are visible
				jumpToPage(currpage + 2);
		}
		else
		{
			jumpToPage(currpage + 1);
		}
	}
}/*}}}*/

void ComicMainWindow::prevPage()/*{{{*/
{
	jumpToPage(currpage - (twoPagesAction->isOn() && cfg->twoPagesStep() ? 2 : 1));
}/*}}}*/

void ComicMainWindow::prevPageBottom()/*{{{*/
{
	if (currpage > 0)
	{
		jumpToPage(currpage - (twoPagesAction->isOn() && cfg->twoPagesStep() ? 2 : 1));
		view->scrollToBottom();
	}
}/*}}}*/

void ComicMainWindow::firstPage()/*{{{*/
{
	jumpToPage(0);
}/*}}}*/

void ComicMainWindow::lastPage()/*{{{*/
{
	if (sink)
		jumpToPage(sink->numOfImages() - (twoPagesAction->isOn() && cfg->twoPagesStep() ? 2 : 1));
}/*}}}*/

void ComicMainWindow::forwardPages()/*{{{*/
{
	jumpToPage(currpage + (twoPagesAction->isOn() && cfg->twoPagesStep() ? 10 : 5));
}/*}}}*/

void ComicMainWindow::backwardPages()/*{{{*/
{
	jumpToPage(currpage - (twoPagesAction->isOn() && cfg->twoPagesStep() ? 10 : 5));
}/*}}}*/

void ComicMainWindow::jumpToPage(int n, bool force)/*{{{*/
{
	if (!sink)
		return;
	if (n >= sink->numOfImages())
			n = sink->numOfImages()-1;
	if (n < 0)
		n = 0;
	if ((n != currpage) || force)
	{
		int result1, result2;
		int preload = cfg->preloadPages() ? 1 : 0;
		
		if (twoPagesAction->isOn())
		{
			QImage img1(sink->getImage(currpage = n, result1, 0)); //get 1st image, don't preload next one
			QImage img2(sink->getImage(currpage + 1, result2, cfg->twoPagesStep() ? 2*preload : preload)); //preload next 2 images
			if (result2 == 0)
			{
				if (mangaModeAction->isOn())
				{
					view->setImage(img2, img1);
					statusbar->setImageInfo(&img2, &img1);
				}
				else
				{
					view->setImage(img1, img2);
					statusbar->setImageInfo(&img1, &img2);
				}
			}
			else
			{
				view->setImage(img1);
				statusbar->setImageInfo(&img1);
			}
		}
		else
		{
			QImage img(sink->getImage(currpage = n, result1, preload)); //preload next image
			view->setImage(img);
			statusbar->setImageInfo(&img);
		}
		const QString page = tr("Page") + " " + QString::number(currpage + 1) + "/" + QString::number(sink->numOfImages());
		pageinfo->setText(page);
		statusbar->setPage(currpage + 1, sink->numOfImages());
		thumbswin->view()->scrollToPage(currpage);
	}
}/*}}}*/

void ComicMainWindow::showInfo()/*{{{*/
{
	if (sink)
	{
		ComicBookInfo *i = new ComicBookInfo(this, *sink, cfg->infoFont());
		i->show();
	}
}/*}}}*/

void ComicMainWindow::showAbout()/*{{{*/
{
	AboutDialog *win = new AboutDialog(this, "About QComicBook",
			"QComicBook " VERSION " - comic book viewer for GNU/Linux\n"
			"(c)by Pawel Stolowski 2005\n"
			"released under terms of GNU General Public License\n\n"
			"http://linux.bydg.org/~yogin\n"
			"yogin@linux.bydg.org");
	win->show();
}/*}}}*/

void ComicMainWindow::showHelp()/*{{{*/
{
	HelpBrowser *help = new HelpBrowser("QComicBook Help", DATADIR "/help");
	help->show();
}/*}}}*/

void ComicMainWindow::showConfigDialog()/*{{{*/
{
	ComicBookCfgDialog *d = new ComicBookCfgDialog(this, cfg);
	d->show();
}/*}}}*/

void ComicMainWindow::showJumpToPage(const QString &number)/*{{{*/
{
	if (sink)
	{
		JumpToPageWindow *win = new JumpToPageWindow(this, number.toInt(), sink->numOfImages());
		connect(win, SIGNAL(pageSelected(int)), this, SLOT(jumpToPage(int)));
		win->show();
	}
}/*}}}*/

void ComicMainWindow::closeSink()/*{{{*/
{
	enableComicBookActions(false);

	view->clear();
	if (sink)
	{
		sink->deleteLater();
		sink = NULL;
	}
	thumbswin->view()->clear();
	updateCaption();
	statusbar->clear();
}/*}}}*/

void ComicMainWindow::setBookmark()/*{{{*/
{
	if (sink)
		bookmarks->set(sink->getFullName(), currpage);
}/*}}}*/

void ComicMainWindow::removeBookmark()/*{{{*/
{
	if (sink && bookmarks->exists(sink->getFullName()) && QMessageBox::question(this, tr("Removing bookmark"),
		    tr("Do you really want to remove bookmark\nfor this comic book?"),
			QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		bookmarks->remove(sink->getFullName());
}/*}}}*/

void ComicMainWindow::openBookmarksManager()/*{{{*/
{
	BookmarkManager *win = new BookmarkManager(this, bookmarks);
	win->show();
}/*}}}*/

void ComicMainWindow::bookmarkSelected(int id)/*{{{*/
{
	Bookmark b;
	if (bookmarks->get(id, b))
	{
		if (b.getName() != QString::null)
		{
			QString fname = b.getName();
			if (sink && fname == sink->getFullName()) //is this comicbook already opened?
			{
				jumpToPage(b.getPage(), true); //if so, just jump to bookmarked page
				return;
			}
				
			QFileInfo finfo(fname);
			if (!finfo.exists())
			{
				if (QMessageBox::question(this, tr("Comic book not found"),
				  tr("Selected bookmark points to\nnon-existing comic book\nDo you want to remove it?"),
				  QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
					bookmarks->remove(id);
				return;
			}
			open(fname, b.getPage());
		}
	}
}/*}}}*/

