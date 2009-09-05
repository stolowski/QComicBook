/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "config.h"
#include "Bookmarks.h"
#include "ComicMainWindow.h"
#include "Page.h"
#include "ComicBookInfo.h"
#include "ComicImageView.h"
#include "ArchiversConfiguration.h"
#include "ImgArchiveSink.h"
#include "ImgSinkFactory.h"
#include "AboutDialog.h"
#include "ComicBookSettings.h"
#include "History.h"
#include "ComicBookCfgDialog.h"
#include "StatusBar.h"
#include "ThumbnailsWindow.h"
#include "ThumbnailsView.h"
#include "ThumbnailLoaderThread.h"
#include "BookmarkManager.h"
#include "Utility.h"
#include "SupportedArchivesWindow.h"
#include "JumpToPageWindow.h"
#include "PageLoaderThread.h"
#include <QMenu>
#include <QStringList>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QToolBar>
#include <QMessageBox>
#include <QLabel>
#include <QKeyEvent>
#include <QWidgetAction>
#include <QList>
#include <QUrl>

using namespace QComicBook;
using namespace Utility;

ComicMainWindow::ComicMainWindow(QWidget *parent): QMainWindow(parent), sink(NULL), currpage(0)
{
    setupUi(this);
    updateCaption();
    setAttribute(Qt::WA_DeleteOnClose);
    
    cfg = &ComicBookSettings::instance();
    cfg->restoreGeometry(this);

    setupThumbnailsWindow();
    setupActions();
    setupStatusbar();
    setupComicImageView();
    
    setupFileMenu();  
    setupViewMenu();
    setupNavigationMenu();
    setupBookmarksMenu();
    setupSettingsMenu();
    setupHelpMenu();
    setupContextMenu();
    
    //
    // copy all menu actions; this is needed for fullscreen mode if menubar is hidden
    addActions(menuBar()->actions());
    addAction(actionScrollRight);
    addAction(actionScrollLeft);
    addAction(actionScrollRightFast);;
    addAction(actionScrollLeftFast);
    addAction(actionScrollUp);
    addAction(actionScrollDown);
    addAction(actionScrollUpFast);
    addAction(actionScrollDownFast);
    addAction(actionJumpDown);
    addAction(actionJumpUp);
    
    lastdir = cfg->lastDir();
    recentfiles = new History(10);
    *recentfiles = cfg->recentlyOpened();
    setRecentFilesMenu(*recentfiles);
    
    cfg->restoreDockLayout(this);
    
    connect(cfg, SIGNAL(displaySettingsChanged()), this, SLOT(reconfigureDisplay()));
    enableComicBookActions(false);

    pageLoader = new PageLoaderThread();
    connect(pageLoader, SIGNAL(pageLoaded(const Page&)), view, SLOT(setImage(const Page&)));
    connect(pageLoader, SIGNAL(pageLoaded(const Page&, const Page&)), view, SLOT(setImage(const Page&, const Page&)));
    connect(view, SIGNAL(requestPage(int)), pageLoader, SLOT(request(int)));
    connect(view, SIGNAL(requestTwoPages(int)), pageLoader, SLOT(requestTwoPages(int)));
    connect(view, SIGNAL(cancelPageRequest(int)), pageLoader, SLOT(cancel(int)));
    connect(view, SIGNAL(cancelTwoPagesRequest(int)), pageLoader, SLOT(cancelTwoPages(int)));
    pageLoader->start();

    thumbnailLoader = new ThumbnailLoaderThread();
    connect(thumbnailLoader, SIGNAL(thumbnailLoaded(const Thumbnail &)), thumbswin, SLOT(setThumbnail(const Thumbnail &)));
    thumbnailLoader->start();
}

ComicMainWindow::~ComicMainWindow()
{
        if (cfg->cacheThumbnails())
                ImgDirSink::removeThumbnails(cfg->thumbnailsAge());

        saveSettings();        

        delete recentfiles;
        delete bookmarks;

        pageLoader->stop();
        thumbnailLoader->stop();
        pageLoader->wait();
        thumbnailLoader->wait();

        delete pageLoader;
        delete thumbnailLoader;

        if (sink)
        {
            delete sink;
        }
}

void ComicMainWindow::setupActions()
{
        QActionGroup *scaleActions = new QActionGroup(this);
        scaleActions->addAction(actionFitWidth);
        scaleActions->addAction(actionFitHeight);
        scaleActions->addAction(actionWholePage);
        scaleActions->addAction(actionOriginalSize);
        scaleActions->addAction(actionBestFit);
       
        actionExitFullScreen = new QAction(QString::null, this);
        actionExitFullScreen->setShortcut(tr("Escape"));
        addAction(actionExitFullScreen);

        actionToggleThumbnails = thumbswin->toggleViewAction();
	actionToggleThumbnails->setIcon(QPixmap(":/icons/thumbnails.png"));
        actionToggleThumbnails->setShortcut(tr("Alt+t"));
        actionToggleThumbnails->setCheckable(true);
        toolBar->insertAction(actionShowInfo, actionToggleThumbnails);

        connect(actionOpenArchive, SIGNAL(triggered(bool)), this, SLOT(browseArchive()));
        connect(actionOpenDirectory, SIGNAL(triggered(bool)), this, SLOT(browseDirectory()));
        connect(actionOpenNext, SIGNAL(triggered(bool)), this, SLOT(openNext()));
        connect(actionOpenPrevious, SIGNAL(triggered(bool)), this, SLOT(openPrevious()));
	connect(actionSavePageAs, SIGNAL(triggered(bool)), this, SLOT(savePageAs()));
        connect(actionShowInfo, SIGNAL(triggered(bool)), this, SLOT(showInfo()));
        connect(actionExitFullScreen, SIGNAL(triggered(bool)), this, SLOT(exitFullscreen()));
        connect(actionNextPage, SIGNAL(triggered(bool)), this, SLOT(nextPage()));
        connect(actionForwardPage, SIGNAL(triggered(bool)), this, SLOT(forwardPages()));
	connect(actionFirstPage, SIGNAL(triggered(bool)), this, SLOT(firstPage()));
	connect(actionLastPage, SIGNAL(triggered(bool)), this, SLOT(lastPage()));
        connect(actionBackwardPage, SIGNAL(triggered(bool)), this, SLOT(backwardPages())); 
	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void ComicMainWindow::setupComicImageView()
{
    view = new ComicImageView(this, 0, actionTwoPages->isChecked(), cfg->pageSize(), cfg->background());
        setCentralWidget(view);
        view->setFocus();
        reconfigureDisplay();
        connect(actionFullscreen, SIGNAL(triggered(bool)), this, SLOT(toggleFullScreen()));
        connect(actionPageTop, SIGNAL(triggered(bool)), view, SLOT(scrollToTop()));
        connect(actionPageBottom, SIGNAL(triggered(bool)), view, SLOT(scrollToBottom()));
        connect(actionScrollRight, SIGNAL(triggered(bool)), view, SLOT(scrollRight()));
        connect(actionScrollLeft, SIGNAL(triggered(bool)), view, SLOT(scrollLeft()));
        connect(actionScrollRightFast, SIGNAL(triggered(bool)), view, SLOT(scrollRightFast()));
        connect(actionScrollLeftFast, SIGNAL(triggered(bool)), view, SLOT(scrollLeftFast()));
        connect(actionScrollUp, SIGNAL(triggered(bool)), view, SLOT(scrollUp()));
        connect(actionScrollDown, SIGNAL(triggered(bool)), view, SLOT(scrollDown()));       
        connect(actionScrollUpFast, SIGNAL(triggered(bool)), view, SLOT(scrollUpFast()));        
        connect(actionScrollDownFast, SIGNAL(triggered(bool)), view, SLOT(scrollDownFast()));
        connect(actionFitWidth, SIGNAL(triggered(bool)), view, SLOT(setSizeFitWidth()));        
        connect(actionFitHeight, SIGNAL(triggered(bool)), view, SLOT(setSizeFitHeight()));        
        connect(actionWholePage, SIGNAL(triggered(bool)), view, SLOT(setSizeWholePage()));        
        connect(actionOriginalSize, SIGNAL(triggered(bool)), view, SLOT(setSizeOriginal()));        
        connect(actionBestFit, SIGNAL(triggered(bool)), view, SLOT(setSizeBestFit()));        
        connect(actionMangaMode, SIGNAL(toggled(bool)), this, SLOT(toggleJapaneseMode(bool)));        
        connect(actionTwoPages, SIGNAL(toggled(bool)), this, SLOT(toggleTwoPages(bool)));
        connect(actionPreviousPage, SIGNAL(triggered(bool)), this, SLOT(prevPage()));       
        connect(actionRotateRight, SIGNAL(triggered(bool)), view, SLOT(rotateRight()));        
        connect(actionRotateLeft, SIGNAL(triggered(bool)), view, SLOT(rotateLeft()));
        connect(actionNoRotation, SIGNAL(triggered(bool)), view, SLOT(resetRotation()));
        connect(actionJumpDown, SIGNAL(triggered()), view, SLOT(jumpDown()));
        connect(actionJumpUp, SIGNAL(triggered()), view, SLOT(jumpUp()));

        if (cfg->continuousScrolling())
        {
                connect(view, SIGNAL(bottomReached()), this, SLOT(nextPage()));
                connect(view, SIGNAL(topReached()), this, SLOT(prevPageBottom()));
        }
	connect(view, SIGNAL(doubleClick()), this, SLOT(nextPage()));
        view->enableScrollbars(cfg->scrollbarsVisible());
        QAction *which = actionOriginalSize;
        switch (cfg->pageSize())
        {
                case FitWidth:  which = actionFitWidth; break;
                case FitHeight: which = actionFitHeight; break;
                case BestFit:   which = actionBestFit; break;
                case WholePage: which = actionWholePage; break;
                case Original:  which = actionOriginalSize; break;
        }
        which->setChecked(true);
}

void ComicMainWindow::setupThumbnailsWindow()
{
        thumbswin = new ThumbnailsWindow(this);
	thumbswin->setObjectName("ThumbnailsWindow");
	thumbswin->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(Qt::LeftDockWidgetArea, thumbswin);
        connect(thumbswin, SIGNAL(requestedPage(int, bool)), this, SLOT(jumpToPage(int, bool)));
        connect(thumbswin, SIGNAL(shown()), this, SLOT(thumbnailsWindowShown()));
}

void ComicMainWindow::setupFileMenu()
{
    menuRecentFiles = new QMenu(tr("Recently opened"), this);
    menuFile->insertMenu(actionSavePageAs, menuRecentFiles);
    menuFile->insertSeparator(actionSavePageAs);
    connect(menuRecentFiles, SIGNAL(triggered(QAction *)), this, SLOT(recentSelected(QAction *)));
    connect(actionClose, SIGNAL(triggered()), this, SLOT(closeSink()));
}

void ComicMainWindow::setupViewMenu()
{
    menuView->insertAction(actionFullscreen, actionToggleThumbnails);
    menuView->insertSeparator(actionFullscreen);
}

void ComicMainWindow::setupNavigationMenu()
{
    connect(actionJumpToPage, SIGNAL(triggered()), this, SLOT(showJumpToPage()));
    connect(actionToggleContinuousScroll, SIGNAL(toggled(bool)), this, SLOT(toggleContinousScroll()));
    actionTwoPages->setChecked(cfg->twoPagesMode());
    actionMangaMode->setChecked(cfg->japaneseMode());
    actionToggleContinuousScroll->setChecked(cfg->continuousScrolling());
}

void ComicMainWindow::setupBookmarksMenu()
{
    bookmarks = new Bookmarks(menuBookmarks);
    connect(actionSetBookmark, SIGNAL(triggered()), this, SLOT(setBookmark()));
    connect(actionRemoveBookmark, SIGNAL(triggered()), this, SLOT(removeBookmark()));
    connect(menuBookmarks, SIGNAL(triggered(QAction *)), this, SLOT(bookmarkSelected(QAction *)));
    connect(actionManageBookmarks, SIGNAL(triggered()), this, SLOT(openBookmarksManager()));
    bookmarks->load();
}

void ComicMainWindow::setupSettingsMenu()
{
    connect(actionToggleScrollbars, SIGNAL(triggered()), this, SLOT(toggleScrollbars()));
    actionToggleScrollbars->setChecked(cfg->scrollbarsVisible());
    connect(actionConfigure, SIGNAL(triggered()), this, SLOT(showConfigDialog()));
    menuSettings->insertAction(actionToggleStatusbar, toolBar->toggleViewAction());
}

void ComicMainWindow::setupHelpMenu()
{
    connect(actionShowSystemInfo, SIGNAL(triggered()), this, SLOT(showSysInfo()));
    connect(actionShowAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
}

void ComicMainWindow::setupStatusbar()
{
        statusbar = new StatusBar(this);      
	setStatusBar(statusbar);
        connect(actionToggleStatusbar, SIGNAL(toggled(bool)), statusbar, SLOT(setVisible(bool)));
        actionToggleStatusbar->setChecked(cfg->showStatusbar());
        statusbar->setShown(cfg->showStatusbar());
}

void ComicMainWindow::setupContextMenu()
{
        QMenu *cmenu = view->contextMenu();
        pageinfo = new QLabel(cmenu);
        pageinfo->setMargin(3);
        pageinfo->setAlignment(Qt::AlignHCenter);
        pageinfo->setFrameStyle(QFrame::Box | QFrame::Raised);
	QWidgetAction *actionPageInfo = new QWidgetAction(this);
	actionPageInfo->setDefaultWidget(pageinfo);

        cmenu->addAction(actionNextPage);
        cmenu->addAction(actionPreviousPage);
        cmenu->addSeparator();
        cmenu->addAction(actionFitWidth);
        cmenu->addAction(actionFitHeight);
       	cmenu->addAction(actionWholePage);
        cmenu->addAction(actionOriginalSize);
        cmenu->addAction(actionBestFit);
        cmenu->addSeparator();
        cmenu->addAction(actionRotateRight);
        cmenu->addAction(actionRotateLeft);
        cmenu->addAction(actionNoRotation);
        cmenu->addSeparator();
        cmenu->addAction(actionTwoPages);
        cmenu->addAction(actionMangaMode);
        cmenu->addSeparator();
        cmenu->addAction(actionFullscreen);
	cmenu->addAction(actionPageInfo);
}

void ComicMainWindow::enableComicBookActions(bool f)
{
        //
        // file menu
        const bool x = f && sink && sink->supportsNext();
        actionClose->setEnabled(f);
        actionShowInfo->setEnabled(f);
        actionOpenNext->setEnabled(x);
        actionOpenPrevious->setEnabled(x);
	actionSavePageAs->setEnabled(x);

        //
        // view menu
        actionRotateRight->setEnabled(f);
        actionRotateLeft->setEnabled(f);
        actionNoRotation->setEnabled(f);

        //
        // navigation menu
	actionLastPage->setEnabled(f);
	actionFirstPage->setEnabled(f);
        actionJumpToPage->setEnabled(f);
        actionNextPage->setEnabled(f);
        actionPreviousPage->setEnabled(f);
        actionBackwardPage->setEnabled(f);
        actionForwardPage->setEnabled(f);
        actionPageTop->setEnabled(f);
        actionPageBottom->setEnabled(f);

        //
        // bookmarks menu
        actionSetBookmark->setEnabled(f);
        actionRemoveBookmark->setEnabled(f);
}

void ComicMainWindow::dragEnterEvent(QDragEnterEvent *e)
{
	if (e->mimeData()->hasUrls())
		e->acceptProposedAction();
}

void ComicMainWindow::dropEvent(QDropEvent *e)
{
	const QList<QUrl> urls = e->mimeData()->urls();
	if (!urls.isEmpty())
		open(urls.at(0).toLocalFile(), 0);
}

void ComicMainWindow::keyPressEvent(QKeyEvent *e)
{
        if ((e->key()>=Qt::Key_1) && (e->key()<=Qt::Key_9))
        {
                e->accept();
                showJumpToPage(e->text());
        }
        else
                QMainWindow::keyPressEvent(e);
}

void ComicMainWindow::closeEvent(QCloseEvent *e)
{
        return (!cfg->confirmExit() || confirmExit()) ? e->accept() : e->ignore();
}

bool ComicMainWindow::confirmExit()
{
        return QMessageBox::question(this, tr("Leave QComicBook?"), tr("Do you really want to quit QComicBook?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
}

void ComicMainWindow::thumbnailsWindowShown()
{
    if (sink)
    {
        int max = sink->numOfImages();
        for (int i=0; i<max; i++)
        {
            if (!thumbswin->view()->isLoaded(i))
            {
                thumbnailLoader->request(i);
            }
        }
    }
}

void ComicMainWindow::toggleScrollbars()
{
        bool f = actionToggleScrollbars->isChecked();
        //actiontoggleScrollbars->setChecked(!f); //???? potrzebne?
        view->enableScrollbars(f);
}

void ComicMainWindow::toggleContinousScroll()
{
        bool f = actionToggleContinuousScroll->isChecked();
        //actionToggleContinuousScroll->setChecked(!f); //???? potrzebne?
        if (f)
        {
                view->disconnect(SIGNAL(bottomReached()), this);
                view->disconnect(SIGNAL(topReached()), this);
        }
        else
        {       connect(view, SIGNAL(bottomReached()), this, SLOT(nextPage()));
                connect(view, SIGNAL(topReached()), this, SLOT(prevPageBottom()));
        }
}

void ComicMainWindow::toggleTwoPages(bool f)
{
//        actionTwoPages->setChecked(f);
    view->setTwoPagesMode(f);
    jumpToPage(currpage, true);
}

void ComicMainWindow::toggleJapaneseMode(bool f)
{
        actionMangaMode->setChecked(f);
        if (actionTwoPages->isChecked())
                jumpToPage(currpage, true);
}

void ComicMainWindow::reloadPage()
{
	if (sink)
		jumpToPage(currpage, true);
}

void ComicMainWindow::updateCaption()
{
        QString c = "QComicBook";
        if (sink)
                c += " - " + sink->getName();
        setWindowTitle(c);
}

void ComicMainWindow::setRecentFilesMenu(const History &hist)
{
        QStringList list = hist.getAll();
        //workaround for crash when removing recent items via clear()
        //when it's called from recentSelected handler.
        QList<QAction *> actions = menuRecentFiles->actions();
        foreach (QAction *a, actions) {
            a->deleteLater();
        }
        for (QStringList::const_iterator it = list.begin(); it != list.end(); it++)
                menuRecentFiles->addAction(*it);
}

void ComicMainWindow::recentSelected(QAction *action) 
{
        const QString &fname = action->text();
        if (fname != QString::null)
        {
                QFileInfo finfo(fname);
                if (!finfo.exists())
                {
                        recentfiles->remove(fname);
                        menuRecentFiles->removeAction(action);
                }
                open(fname, 0);
        }
}

void ComicMainWindow::sinkReady(const QString &path)
{
	statusbar->setShown(actionToggleStatusbar->isChecked() && !(isFullScreen() && cfg->fullScreenHideStatusbar())); //applies back user's statusbar&toolbar preferences
	//toolbar->setShown(actiontoggleToolbar->isOn() && !(isFullScreen() && cfg->fullScreenHideToolbar()));

        recentfiles->append(path);
        setRecentFilesMenu(*recentfiles);

        enableComicBookActions(true);
        updateCaption();
        statusbar->setName(sink->getFullName());

        view->setNumOfPages(sink->numOfImages()); //FIXME
        thumbswin->view()->setPages(sink->numOfImages());

        //
        // request thumbnails for all pages
        if (thumbswin->isVisible())
        {
            thumbnailLoader->request(0, sink->numOfImages());
        }
        
        jumpToPage(currpage, true);

	const bool hasdesc = (sink->getDescription().count() > 0);
	actionShowInfo->setDisabled(!hasdesc);

        if (hasdesc && cfg->autoInfo())
                showInfo();
}

void ComicMainWindow::sinkError(int code)
{
	statusbar->setShown(actionToggleStatusbar->isChecked() && !(isFullScreen() && cfg->fullScreenHideStatusbar())); //applies back user's statusbar&toolbar preferences
	//toolbar->setShown(actiontoggleToolbar->isOn() && !(isFullScreen() && cfg->fullScreenHideToolbar()));

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
        QMessageBox::critical(this, tr("QComicBook error"), tr("Error opening comicbook") + ": " + msg, 
                        QMessageBox::Ok, QMessageBox::NoButton);
        closeSink();
}

void ComicMainWindow::browseDirectory()
{
        const QString dir = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), lastdir);
        if (!dir.isEmpty())
                open(dir, 0);
}

void ComicMainWindow::browseArchive()
{
    const QString file = QFileDialog::getOpenFileName(this, tr("Choose a file"), lastdir, "Archives (" + ArchiversConfiguration::instance().supportedOpenExtensions().join(" ") + ");;All files (*)");
        if (!file.isEmpty())
                open(file, 0);
}

void ComicMainWindow::open(const QString &path, int page)
{
        const QFileInfo f(path);
        const QString fullname = f.absoluteFilePath();

        if (sink && sink->getFullName() == fullname) //trying to open same dir?
                return;

        lastdir = f.absolutePath();
        currpage = page;

        closeSink();

        sink = ImgSinkFactory::instance().createImgSink(path);
	sink->setCacheSize(cfg->cacheSize()*1024*1024, cfg->cacheAutoAdjust());

        pageLoader->setSink(sink);
        thumbnailLoader->setSink(sink);
        thumbnailLoader->setUseCache(cfg->cacheThumbnails());

        connect(sink, SIGNAL(progress(int, int)), statusbar, SLOT(setProgress(int, int)));

        statusbar->setShown(true); //ensures status bar is visible when opening regardless of user settings

        int status = sink->open(fullname);
	if (status)
		sinkError(status);
	else
		sinkReady(fullname);
}

void ComicMainWindow::openNext()
{
	if (sink && sink->supportsNext())
        {
		QString fname = sink->getNext();
		if (!fname.isEmpty())
			open(fname, 0);
        }
}

void ComicMainWindow::openPrevious()
{
	if (sink && sink->supportsNext())
        {
		QString fname = sink->getPrevious();
		if (!fname.isEmpty())
                        open(fname, 0);
        }
}

void ComicMainWindow::toggleFullScreen()
{
        if (isFullScreen())
        {
                exitFullscreen();
        }
        else
        {
		savedToolbarState = toolBar->toggleViewAction()->isChecked();

                if (cfg->fullScreenHideMenu())
                        menuBar()->hide();
                if (cfg->fullScreenHideStatusbar())
                        statusbar->hide();
                if (cfg->fullScreenHideToolbar())
		{
			toolBar->toggleViewAction()->setChecked(false);
                        toolBar->hide();
		}
                showFullScreen();
        }
}

void ComicMainWindow::exitFullscreen()
{
        if (isFullScreen())
        {
                menuBar()->show();
                if (actionToggleStatusbar->isChecked())
                        statusbar->show();
		toolBar->toggleViewAction()->setChecked(savedToolbarState);
		if (savedToolbarState)
			toolBar->show();
		else
			toolBar->hide();
		showNormal();
        }
}

void ComicMainWindow::nextPage()
{
        if (sink)
        {
                if (actionTwoPages->isChecked() && cfg->twoPagesStep())
                {
                        if (currpage < sink->numOfImages() - 2) //do not change pages if last two pages are visible
                                jumpToPage(currpage + 2);
                }
                else
                {
                        jumpToPage(currpage + 1);
                }
        }
}

void ComicMainWindow::prevPage()
{
        jumpToPage(currpage - (actionTwoPages->isChecked() && cfg->twoPagesStep() ? 2 : 1));
}

void ComicMainWindow::prevPageBottom()
{
        if (currpage > 0)
        {
                jumpToPage(currpage - (actionTwoPages->isChecked() && cfg->twoPagesStep() ? 2 : 1));
                view->scrollToBottom();
        }
}

void ComicMainWindow::firstPage()
{
        jumpToPage(0);
}

void ComicMainWindow::lastPage()
{
        if (sink)
                jumpToPage(sink->numOfImages() - (actionTwoPages->isChecked() && cfg->twoPagesStep() ? 2 : 1));
}

void ComicMainWindow::forwardPages()
{
        jumpToPage(currpage + (actionTwoPages->isChecked() && cfg->twoPagesStep() ? 10 : 5));
}

void ComicMainWindow::backwardPages()
{
        jumpToPage(currpage - (actionTwoPages->isChecked() && cfg->twoPagesStep() ? 10 : 5));
}

void ComicMainWindow::jumpToPage(int n, bool force)
{
	//
	// enable or disable next/prev/backward/forward page actions if first/last page shown
        if (sink == NULL || n == sink->numOfImages() - (actionTwoPages->isChecked() ? 2 : 1))
	{
		actionNextPage->setDisabled(true);
		actionForwardPage->setDisabled(true);
	}
	else
	{
		actionNextPage->setDisabled(false);
		actionForwardPage->setDisabled(false);
	}
        if (sink == NULL || n == (actionTwoPages->isChecked() ? 1 : 0))
	{
                actionPreviousPage->setDisabled(true);
		actionBackwardPage->setDisabled(true);
	}
	else
	{
		actionPreviousPage->setDisabled(false);
		actionBackwardPage->setDisabled(false);
	}

	if (!sink)
		return;
        if (n >= sink->numOfImages())
                n = sink->numOfImages()-1;
        if (n < 0)
                n = 0;
        if ((n != currpage) || force)
        {
                int result1, result2;
                const bool preserveangle = actionTogglePreserveRotation->isChecked(); //FIXME

                if (actionTwoPages->isChecked())
                {
                        Page img1 = sink->getImage(currpage = n, result1);
                        Page img2 = sink->getImage(currpage + 1, result2);
                        if (result2 == 0)
                        {
                                if (actionMangaMode->isChecked())
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
				//if (cfg->preloadPages()) TODO
				//	sink->preload(currpage + 1);
                        }
                }
                else
                {
                        Page img = sink->getImage(currpage = n, result1);
                        view->setImage(img);
                        statusbar->setImageInfo(&img);
                }
                if (actionMangaMode->isChecked())
                        view->ensureVisible(view->viewWidth(), 0);
		else
			view->ensureVisible(0, 0);
                const QString page = tr("Page") + " " + QString::number(currpage + 1) + "/" + QString::number(sink->numOfImages());
                pageinfo->setText(page);
                statusbar->setPage(currpage + 1, sink->numOfImages());
                thumbswin->view()->scrollToPage(currpage);
		if (cfg->preloadPages())
		{
			if (!actionTwoPages->isChecked())
			{
                            //sink->preload(currpage + 1);
			}
			else
			{
                            //sink->preload(currpage + 3);
                            //sink->preload(currpage + 2);
			}
		}
        }
}

void ComicMainWindow::showInfo()
{
        if (sink)
        {
                ComicBookInfo *i = new ComicBookInfo(this, *sink, cfg->infoFont());
                i->show();
        }
}

void ComicMainWindow::showSysInfo()
{
	SupportedArchivesWindow *win = new SupportedArchivesWindow(this);
	win->show();
}

void ComicMainWindow::showAbout()
{
        AboutDialog *win = new AboutDialog(this, "About QComicBook",
                        "QComicBook " VERSION " - comic book viewer for GNU/Linux\n"
                        "(c)by Pawel Stolowski 2005-2009\n"
                        "released under terms of GNU General Public License\n\n"
                        "http://linux.bydg.org/~yogin\n"
                        "stolowski@gmail.com", QPixmap(DATADIR "/qcomicbook-splash.png"));
        win->show();
}

void ComicMainWindow::showConfigDialog()
{
        ComicBookCfgDialog *d = new ComicBookCfgDialog(this, cfg);
        d->show();
}

void ComicMainWindow::showJumpToPage(const QString &number)
{
        if (sink)
        {
                JumpToPageWindow *win = new JumpToPageWindow(this, number.toInt(), sink->numOfImages());
                connect(win, SIGNAL(pageSelected(int)), this, SLOT(jumpToPage(int)));
                win->show();
        }
}

void ComicMainWindow::closeSink()
{
    enableComicBookActions(false);

    if (sink)
    {
        pageLoader->cancelAll();
        pageLoader->setSink(NULL);
        thumbnailLoader->cancelAll();
        thumbnailLoader->setSink(NULL);
        sink->deleteLater();
        sink = NULL;
    }
    view->clear();
    thumbswin->view()->clear();
    updateCaption();
    statusbar->clear();
}

void ComicMainWindow::setBookmark()
{
        if (sink)
                bookmarks->set(sink->getFullName(), currpage);
}

void ComicMainWindow::removeBookmark()
{
        if (sink && bookmarks->exists(sink->getFullName()) && QMessageBox::question(this, tr("Removing bookmark"),
                                tr("Do you really want to remove bookmark\nfor this comic book?"),
                                QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
                bookmarks->remove(sink->getFullName());
}

void ComicMainWindow::openBookmarksManager()
{
        BookmarkManager *win = new BookmarkManager(this, bookmarks);
        win->show();
}

void ComicMainWindow::savePageAs()
{
	if (sink && (sink->numOfImages() > 0))
	{
		const QString msg = tr("Save image as");
		int cnt = view->visiblePages();
		for (int i = 1; i<=cnt; i++)
		{
			QString tmpmsg(msg);
			if (cnt > 1)
				tmpmsg += " (" + tr("page") + " " + QString::number(i) + ")";
			QString fname = QFileDialog::getSaveFileName(this, tmpmsg, QString::null, "Images (*.jpg *.png)");
			if (!fname.isEmpty())
			{
				int result;
				const Page img = sink->getImage(currpage, result);
				if (result != 0 || !img.getImage().save(fname)) //TODO: overwrite and default format (jpeg)
				{
					QMessageBox::critical(this, tr("QComicBook error"), tr("Error saving image"), QMessageBox::Ok, QMessageBox::NoButton);
					break; //do not attempt to save second image
				}
			}
		}
	}
}

void ComicMainWindow::bookmarkSelected(QAction *action) 
{
        Bookmark b;
        if (bookmarks->get(action, b))
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
                                        bookmarks->remove(action);
                                return;
                        }
                        open(fname, b.getPage());
                }
        }
}

void ComicMainWindow::saveSettings()
{
        cfg->saveGeometry(this);
        cfg->saveDockLayout(this);
        cfg->scrollbarsVisible(actionToggleScrollbars->isChecked());
        cfg->twoPagesMode(actionTwoPages->isChecked());
        cfg->japaneseMode(actionMangaMode->isChecked());
        cfg->continuousScrolling(actionToggleContinuousScroll->isChecked());
        cfg->lastDir(lastdir);
        cfg->recentlyOpened(*recentfiles);
        cfg->pageSize(view->getSize());
        cfg->showStatusbar(actionToggleStatusbar->isChecked());

        bookmarks->save();        
}

void ComicMainWindow::reconfigureDisplay()
{
    view->setSmallCursor(cfg->smallCursor());
    view->showPageNumbers(cfg->embedPageNumbers());
    view->setBackground(cfg->background());
}
