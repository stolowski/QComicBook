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
#include "imgdirsink.h"
#include "aboutdialog.h"
#include "cbsettings.h"
#include "history.h"
#include "helpbrowser.h"
#include "cbconfigdialog.h"
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
#include <qdockwindow.h>
#include <qprogressdialog.h>
#include "thumbnailswin.h"
#include "thumbnailsview.h"
#include "thumbnailloader.h"
#include "statusbar.h"

//
// archives extensions used for Open File dialog and filtering out files in OpenNext() function
const QString ComicMainWindow::ARCH_EXTENSIONS = "*.rar *.cbr *.zip *.cbz *.ace *.cba *.tar.gz *.tgz *.cbg *.tar.bz2 *.cbb";

ComicMainWindow::ComicMainWindow(QWidget *parent): QMainWindow(parent, NULL, WType_TopLevel|WDestructiveClose), sink(NULL), currpage(0)/*{{{*/
{
	bool f; 

	updateCaption();
	setIcon(Icons::get(ICON_APPICON).pixmap(QIconSet::Small, true));
	setMinimumSize(320, 200);

	recentfiles = new History(10);
	cfg = &ComicBookSettings::instance();

	setGeometry(cfg->getGeometry());

	//
	// statusbar
	statusbar = new StatusBar(this);
	toggleStatusbarAction = new QAction(tr("Statusbar"), QKeySequence(), this);
	toggleStatusbarAction->setToggleAction(true);
	connect(toggleStatusbarAction, SIGNAL(toggled(bool)), statusbar, SLOT(setShown(bool)));
	toggleStatusbarAction->setOn(cfg->getShowStatusbar());
	statusbar->setShown(cfg->getShowStatusbar());
	
	//
	// comic view
	view = new ComicImageView(this, cfg->getPageSize(), cfg->getScaling(), cfg->getBackground());
	setCentralWidget(view);
	view->setFocus();
	view->setSmallCursor(cfg->getUseSmallCursor());
	connect(cfg, SIGNAL(backgroundChanged(const QColor&)), view, SLOT(setBackground(const QColor&)));
	connect(cfg, SIGNAL(scalingMethodChanged(ComicImageView::Scaling)), view, SLOT(setScaling(ComicImageView::Scaling)));

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
	mangaModeAction = new QAction(tr("Japanese mode"), CTRL+Key_J, this);
	mangaModeAction->setToggleAction(true);
	connect(mangaModeAction, SIGNAL(toggled(bool)), this, SLOT(toggleJapaneseMode(bool)));
	twoPagesAction = new QAction(tr("Two pages"), CTRL+Key_T, this);
	twoPagesAction->setToggleAction(true);
	connect(twoPagesAction, SIGNAL(toggled(bool)), this, SLOT(toggleTwoPages(bool)));
	
	showInfoAction = new QAction(Icons::get(ICON_INFO), tr("Info"), ALT+Key_I, this);
	connect(showInfoAction, SIGNAL(activated()), this, SLOT(showInfo()));
	QAction *exitFullscreenAction = new QAction(QString::null, Key_Escape, this);
	connect(exitFullscreenAction, SIGNAL(activated()), this, SLOT(exitFullscreen()));
		
	QAction *which = originalSizeAction;
	switch (cfg->getPageSize())
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
	fullScreenAction->addTo(view->contextMenu());
	view->contextMenu()->insertSeparator();
	view->contextMenu()->insertItem(pageinfo);
	
	//
	// File menu
	file_menu = new QPopupMenu(this);
	openDirAction->addTo(file_menu);
	openArchiveAction->addTo(file_menu);
	opennext_id = file_menu->insertItem(tr("Open next"), this, SLOT(openNext()));
	openprv_id = file_menu->insertItem(tr("Open previous"), this, SLOT(openPrevious()));
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
	toggleToolbarAction->addTo(view_menu);
	view->enableScrollbars(f);
	toggleStatusbarAction->addTo(view_menu);
	menuBar()->insertItem(tr("&View"), view_menu);
	
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
	
	view_menu->setItemChecked(scrv_id, f = cfg->getScrollbarsVisible());
	twoPagesAction->setOn(cfg->getTwoPagesMode());
	mangaModeAction->setOn(cfg->getJapaneseMode());
	navi_menu->setItemChecked(contscr_id, cfg->getContinuousScrolling());

	if (cfg->getContinuousScrolling())
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
	bookmarks_menu->insertSeparator();
	bookmarks->load();
	connect(bookmarks_menu, SIGNAL(activated(int)), this, SLOT(bookmarkSelected(int)));
	
	//
	// Help menu
	QPopupMenu *help_menu = new QPopupMenu(this);
	help_menu->insertItem(tr("Index"), this, SLOT(showHelp()));
	help_menu->insertItem(tr("About"), this, SLOT(showAbout()));
	menuBar()->insertItem(tr("&Help"), help_menu);
	
	lastdir = cfg->getLastDir();
	*recentfiles = cfg->getRecentlyOpened();
	setRecentFilesMenu(*recentfiles);

	cfg->restoreDockLayout(this);

	enableComicBookActions(false);
}/*}}}*/

ComicMainWindow::~ComicMainWindow()/*{{{*/
{
	if (cfg->getCacheThumbnails())
		ImgDirSink::removeThumbnails(cfg->getThumbnailsAge());

	cfg->setGeometry(frameGeometry());
	cfg->saveDockLayout(this);
	cfg->setScrollbarsVisible(view_menu->isItemChecked(scrv_id));
	cfg->setTwoPagesMode(twoPagesAction->isOn());
	cfg->setJapaneseMode(mangaModeAction->isOn());
	cfg->setContinuousScrolling(navi_menu->isItemChecked(contscr_id));
	cfg->setLastDir(lastdir);
	cfg->setRecentlyOpened(*recentfiles);
	cfg->setPageSize(view->getSize());
	cfg->setShowStatusbar(toggleStatusbarAction->isOn());
	
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
	file_menu->setItemEnabled(opennext_id, x);
	file_menu->setItemEnabled(openprv_id, x);
	file_menu->setItemEnabled(close_id, f);
	showInfoAction->setEnabled(f);
	
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
	return (!cfg->getConfirmExit() || confirmExit()) ? e->accept() : e->ignore();
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
		currpage = 0;
		if (finfo.isDir())
			openDir(fname);
		else
			openArchive(fname);
	}
}/*}}}*/

void ComicMainWindow::sinkReady(const QString &path)/*{{{*/
{
	recentfiles->append(path);
	setRecentFilesMenu(*recentfiles);

	updateCaption();
	statusbar->setName(sink->getFullName());

	thumbswin->view()->setPages(sink->numOfImages());

	//
	// request thumbnails for all pages
	if (thumbswin->isVisible())
		sink->requestThumbnails(0, sink->numOfImages());

	jumpToPage(currpage, true);
	if (cfg->getAutoInfo())
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
	{
		currpage = 0;
		openDir(dir);
	}
}/*}}}*/

void ComicMainWindow::browseArchive()/*{{{*/
{
	const QString file = QFileDialog::getOpenFileName(lastdir,
			"Archives (" + ARCH_EXTENSIONS + ");;All files (*)",
			this, NULL, tr("Choose a file") );
	if (!file.isEmpty())
	{
		currpage = 0;
		openArchive(file);
	}
}/*}}}*/

void ComicMainWindow::open(const QString &path)/*{{{*/
{
	QFileInfo finfo(path);
	if (finfo.isDir())
		openDir(path);
	else
		openArchive(path);
}/*}}}*/

void ComicMainWindow::openDir(const QString &name)/*{{{*/
{
	lastdir = name;
	
	if (sink && sink->getFullName() == name) //trying to open same dir?
		return;
	
	closeSink();

	ImgDirSink *dsink = new ImgDirSink(cfg->getCacheSize());
	sink = dsink;
	dsink->thumbnailLoader().setReciever(thumbswin);
	dsink->thumbnailLoader().setUseCache(cfg->getCacheThumbnails());
	connect(sink, SIGNAL(sinkReady(const QString&)), this, SLOT(sinkReady(const QString&)));
	connect(sink, SIGNAL(sinkError(int)), this, SLOT(sinkError(int)));
	
	QProgressDialog *win = new QProgressDialog(tr("Please wait. Reading directory"), 0, 1, this, 0, true);
	win->setAutoClose(false);
	win->setAutoReset(false);
	win->setCaption(caption());
	win->show();

	connect(sink, SIGNAL(sinkReady(const QString&)), win, SLOT(close()));
	connect(sink, SIGNAL(sinkError(int)), win, SLOT(close()));
	connect(sink, SIGNAL(progress(int, int)), win, SLOT(setProgress(int, int)));

	sink->open(name);
	
	enableComicBookActions(true);
}/*}}}*/

void ComicMainWindow::openArchive(const QString &name)/*{{{*/
{
	if (sink && sink->getFullName() == name) //trying to open same dir?
		return;

	QFileInfo f(name);
	lastdir = f.dirPath(true);

	closeSink();

	ImgArchiveSink *asink = new ImgArchiveSink(cfg->getCacheSize());
	sink = asink;
	asink->thumbnailLoader().setReciever(thumbswin);
	asink->thumbnailLoader().setUseCache(cfg->getCacheThumbnails());
	connect(sink, SIGNAL(sinkReady(const QString&)), this, SLOT(sinkReady(const QString&)));
	connect(sink, SIGNAL(sinkError(int)), this, SLOT(sinkError(int)));
	
	QProgressDialog *win = new QProgressDialog(tr("Please wait. Decompressing archive"), 0, 1, this, 0, true);
	win->setCaption(caption());
	win->setAutoClose(false);
	win->setAutoReset(false);
	win->show();

	connect(sink, SIGNAL(sinkReady(const QString&)), win, SLOT(close()));
	connect(sink, SIGNAL(sinkError(int)), win, SLOT(close()));
	connect(sink, SIGNAL(progress(int, int)), win, SLOT(setProgress(int, int)));

	sink->open(name);
	
	enableComicBookActions(true);
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
				openArchive(dir.filePath(*it, true));
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
			openArchive(dir.filePath(*(--it), true));
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
		if (cfg->getFullScreenHideMenu())
			menuBar()->hide();
		/*if (cfg->getFullScreenHideToolbar())
			toolbar->hide();
		if (cfg->getFullScreenHideScrollbars())
			view->enableScrollbars(false);*/
		showFullScreen();
	}
}/*}}}*/

void ComicMainWindow::exitFullscreen()/*{{{*/
{
	if (isFullScreen())
	{
		menuBar()->show();
		/*if (view_menu->isItemChecked(toolbar_id))
			toolbar->show();
		if (view_menu->isItemChecked(scrv_id))
			view->enableScrollbars(true);*/
		showNormal();
	}
}/*}}}*/

void ComicMainWindow::nextPage()/*{{{*/
{
	jumpToPage(currpage + (twoPagesAction->isOn() && cfg->getTwoPagesStep() ? 2 : 1));
}/*}}}*/

void ComicMainWindow::prevPage()/*{{{*/
{
	jumpToPage(currpage - (twoPagesAction->isOn() && cfg->getTwoPagesStep() ? 2 : 1));
}/*}}}*/

void ComicMainWindow::prevPageBottom()/*{{{*/
{
	if (currpage > 0)
	{
		jumpToPage(currpage - (twoPagesAction->isOn() && cfg->getTwoPagesStep() ? 2 : 1));
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
		jumpToPage(sink->numOfImages() - (twoPagesAction->isOn() && cfg->getTwoPagesStep() ? 2 : 1));
}/*}}}*/

void ComicMainWindow::forwardPages()/*{{{*/
{
	jumpToPage(currpage + (twoPagesAction->isOn() && cfg->getTwoPagesStep() ? 10 : 5));
}/*}}}*/

void ComicMainWindow::backwardPages()/*{{{*/
{
	jumpToPage(currpage - (twoPagesAction->isOn() && cfg->getTwoPagesStep() ? 10 : 5));
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
		int preload = cfg->getPreload() ? 1 : 0;
		
		if (twoPagesAction->isOn())
		{
			QImage img1(sink->getImage(currpage = n, result1, 0)); //get 1st image, don't preload next one
			QImage img2(sink->getImage(currpage + 1, result2, cfg->getTwoPagesStep() ? 2*preload : preload)); //preload next 2 images
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
		ComicBookInfo *i = new ComicBookInfo(this, *sink, cfg->getFont());
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
		sink->deleteLater();
	sink = NULL;
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
			currpage = b.getPage();
			if (finfo.isDir())
				openDir(fname);
			else
				openArchive(fname);
		}
	}
}/*}}}*/

