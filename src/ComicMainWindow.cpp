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
#include "ContinuousPageView.h"
#include "SimplePageView.h"
#include "FrameView.h"
#include "Archivers/ArchiversConfiguration.h"
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
#include "SystemInfoDialog.h"
#include "GoToPageWidget.h"
#include "PageLoaderThread.h"
#include "RecentFilesMenu.h"
#include "PrinterThread.h"
#include <FrameDetectThread.h>
#include "PrintProgressDialog.h"
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
#include <QPrinter>
#include <QPrintDialog>
#include <QDebug>

using namespace QComicBook;
using namespace Utility;

ComicMainWindow::ComicMainWindow(QWidget *parent): QMainWindow(parent), view(NULL), sink(NULL), currpage(0)
{
    setupUi(this);
    updateCaption();
    setAttribute(Qt::WA_DeleteOnClose);
    
    cfg = &ComicBookSettings::instance();
    cfg->restoreGeometry(this);

    printer = new QPrinter();

    pageLoader = new PageLoaderThread();
    frameDetect = new FrameDetectThread();

    //
    // Thumbnails window
    thumbswin = new ThumbnailsWindow(this);
    thumbswin->setObjectName("ThumbnailsWindow");
    thumbswin->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea, thumbswin);
    connect(thumbswin, SIGNAL(requestedPage(int, bool)), this, SLOT(jumpToPage(int, bool)));
    connect(thumbswin, SIGNAL(shown()), this, SLOT(thumbnailsWindowShown()));

    //
    // Actions
    QActionGroup *scaleActions = new QActionGroup(this);
    scaleActions->addAction(actionFitWidth);
    scaleActions->addAction(actionFitHeight);
    scaleActions->addAction(actionWholePage);
    scaleActions->addAction(actionOriginalSize);
    scaleActions->addAction(actionBestFit);
    connect(scaleActions, SIGNAL(triggered(QAction *)), this, SLOT(setPageSize(QAction *)));

    QActionGroup *viewTypeActions = new QActionGroup(this);
    viewTypeActions->addAction(actionContinuousView);
    viewTypeActions->addAction(actionSimpleView);
    viewTypeActions->addAction(actionFrameView);
    connect(viewTypeActions, SIGNAL(triggered(QAction *)), this, SLOT(changeViewType(QAction *)));
       
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
    connect(actionPrint, SIGNAL(triggered(bool)), this, SLOT(openPrintDialog()));
    connect(actionSavePageAs, SIGNAL(triggered(bool)), this, SLOT(savePageAs()));
    connect(actionShowInfo, SIGNAL(triggered(bool)), this, SLOT(showInfo()));
    connect(actionExitFullScreen, SIGNAL(triggered(bool)), this, SLOT(exitFullscreen()));
    connect(actionNextPage, SIGNAL(triggered(bool)), this, SLOT(nextPage()));
    connect(actionForwardPage, SIGNAL(triggered(bool)), this, SLOT(forwardPages()));
    connect(actionFirstPage, SIGNAL(triggered(bool)), this, SLOT(firstPage()));
    connect(actionLastPage, SIGNAL(triggered(bool)), this, SLOT(lastPage()));
    connect(actionBackwardPage, SIGNAL(triggered(bool)), this, SLOT(backwardPages())); 
    connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(actionFullscreen, SIGNAL(triggered(bool)), this, SLOT(toggleFullScreen()));
    connect(actionPreviousPage, SIGNAL(triggered(bool)), this, SLOT(prevPage()));   
    connect(actionMangaMode, SIGNAL(toggled(bool)), this, SLOT(toggleJapaneseMode(bool)));        
    connect(actionTwoPages, SIGNAL(toggled(bool)), this, SLOT(toggleTwoPages(bool)));
    connect(actionNextFrame, SIGNAL(triggered(bool)), this, SLOT(nextFrame()));
    connect(actionPreviousFrame, SIGNAL(triggered(bool)), this, SLOT(prevFrame()));

    //
    // Statusbar
    statusbar = new StatusBar(this);      
    setStatusBar(statusbar);
    connect(actionToggleStatusbar, SIGNAL(toggled(bool)), statusbar, SLOT(setVisible(bool)));
    actionToggleStatusbar->setChecked(cfg->showStatusbar());
    statusbar->setShown(cfg->showStatusbar());

    setupComicImageView();

    //
    // File menu
    menuRecentFiles = new RecentFilesMenu(tr("Recently opened"), this, 10);
    menuFile->insertMenu(actionOpenNext, menuRecentFiles);
    connect(menuRecentFiles, SIGNAL(selected(const QString &)), this, SLOT(recentSelected(const QString &)));
    connect(menuRecentFiles, SIGNAL(cleanupRequest()), menuRecentFiles, SLOT(removeAll()));
    connect(actionClose, SIGNAL(triggered()), this, SLOT(closeSink()));

    // View menu
    menuView->insertAction(actionFullscreen, actionToggleThumbnails);
    menuView->insertSeparator(actionFullscreen);

    //
    // Navigation menu
    connect(actionJumpToPage, SIGNAL(triggered()), this, SLOT(showJumpToPage()));
    connect(actionContinuousView, SIGNAL(toggled(bool)), this, SLOT(toggleContinousView(bool)));
    actionTwoPages->setChecked(cfg->twoPagesMode());
    actionMangaMode->setChecked(cfg->japaneseMode());
    actionContinuousView->setChecked(cfg->continuousScrolling());

    //
    // Bookmarks menu
    bookmarks = new Bookmarks(menuBookmarks);
    connect(actionSetBookmark, SIGNAL(triggered()), this, SLOT(setBookmark()));
    connect(actionRemoveBookmark, SIGNAL(triggered()), this, SLOT(removeBookmark()));
    connect(menuBookmarks, SIGNAL(triggered(QAction *)), this, SLOT(bookmarkSelected(QAction *)));
    connect(actionManageBookmarks, SIGNAL(triggered()), this, SLOT(openBookmarksManager()));
    bookmarks->load();

    //
    // Settings menu
    connect(actionToggleScrollbars, SIGNAL(toggled(bool)), this, SLOT(toggleScrollbars(bool)));
    actionToggleScrollbars->setChecked(cfg->scrollbarsVisible());
    connect(actionConfigure, SIGNAL(triggered()), this, SLOT(showConfigDialog()));
    menuSettings->insertAction(actionToggleStatusbar, toolBar->toggleViewAction());    

    //
    // Help menu
    connect(actionShowSystemInfo, SIGNAL(triggered()), this, SLOT(showSysInfo()));
    connect(actionShowAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

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

    which = actionContinuousView;
    switch (cfg->viewType())
    {
    	case Simple: which = actionSimpleView; break;
	case Continuous: which = actionContinuousView; break;
	case Frame: which = actionFrameView; break;
    }
    which->setChecked(true);
    
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
    menuRecentFiles->set(cfg->recentlyOpened());
    
    cfg->restoreDockLayout(this);
    
    //connect(cfg, SIGNAL(displaySettingsChanged(const QString &)), this, SLOT(reconfigureDisplay())); ??
    enableComicBookActions(false);
    
    pageLoader->start();
    frameDetect->start();

    thumbnailLoader = new ThumbnailLoaderThread();
    connect(thumbnailLoader, SIGNAL(thumbnailLoaded(const Thumbnail &)), thumbswin, SLOT(setThumbnail(const Thumbnail &)));
    thumbnailLoader->start();
}

ComicMainWindow::~ComicMainWindow()
{
    if (cfg->cacheThumbnails())
    {
        ImgDirSink::removeThumbnails(cfg->thumbnailsAge());
    }

    saveSettings();        
    
    delete bookmarks;
    
    frameDetect->stop();
    pageLoader->stop();
    thumbnailLoader->stop();
    pageLoader->wait();
    thumbnailLoader->wait();
    
    delete printer;
    delete pageLoader;
    delete thumbnailLoader;
    
    if (sink)
    {
        delete sink;
    }
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

void ComicMainWindow::setupComicImageView()
{
    const int n = (sink != NULL ? sink->numOfImages() : 0);
    if (view)
    {
        view->disconnect();
        pageLoader->disconnect();
        view->deleteLater();
    }

    const ViewProperties props;
    actionNextFrame->setDisabled(true);
    actionPreviousFrame->setDisabled(true);
    actionTwoPages->setDisabled(false);
    actionMangaMode->setDisabled(!cfg->twoPagesMode());
    switch (cfg->viewType())
    {
	case Continuous:
	        view = new ContinuousPageView(this, n, props);
		frameDetect->clear();
		break;
	case Simple:
		view = new SimplePageView(this, n, props);
		frameDetect->clear();
		break;
	case Frame:
		view = new FrameView(this, n, props);
		actionNextFrame->setDisabled(false);
		actionPreviousFrame->setDisabled(false);
		actionTwoPages->setDisabled(true);
		actionMangaMode->setDisabled(false);
		//
		// connect frame processing thread
		connect(pageLoader, SIGNAL(pageLoaded(const Page &)), frameDetect, SLOT(process(const Page &)));
		connect(frameDetect, SIGNAL(framesReady(const ComicFrameList &)), view, SLOT(setFrames(const ComicFrameList &)));
		break;
	default:
		break;
    }
    
    setCentralWidget(view);
    view->setFocus();

    reconfigureDisplay();
    
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
    connect(actionRotateRight, SIGNAL(triggered(bool)), view, SLOT(rotateRight()));        
    connect(actionRotateLeft, SIGNAL(triggered(bool)), view, SLOT(rotateLeft()));
    connect(actionNoRotation, SIGNAL(triggered(bool)), view, SLOT(resetRotation()));
    connect(actionJumpDown, SIGNAL(triggered()), view, SLOT(jumpDown()));
    connect(actionJumpUp, SIGNAL(triggered()), view, SLOT(jumpUp()));
    
    connect(view, SIGNAL(bottomReached()), this, SLOT(nextPage()));
    connect(view, SIGNAL(topReached()), this, SLOT(prevPageBottom()));
        
    connect(view, SIGNAL(doubleClick()), this, SLOT(nextPage()));
    view->enableScrollbars(cfg->scrollbarsVisible());

    connect(view, SIGNAL(currentPageChanged(int)), this, SLOT(currentPageChanged(int)));
    connect(pageLoader, SIGNAL(pageLoaded(const Page&)), view, SLOT(setImage(const Page&)));
    connect(pageLoader, SIGNAL(pageLoaded(const Page&, const Page&)), view, SLOT(setImage(const Page&, const Page&)));
    connect(view, SIGNAL(pageReady(const Page &)), this, SLOT(pageLoaded(const Page &)));
    connect(view, SIGNAL(pageReady(const Page &, const Page &)), this, SLOT(pageLoaded(const Page &, const Page &)));
    connect(view, SIGNAL(requestPage(int)), pageLoader, SLOT(request(int)));
    connect(view, SIGNAL(requestTwoPages(int)), pageLoader, SLOT(requestTwoPages(int)));
    connect(view, SIGNAL(cancelPageRequest(int)), pageLoader, SLOT(cancel(int)));
    connect(view, SIGNAL(cancelTwoPagesRequest(int)), pageLoader, SLOT(cancelTwoPages(int)));

    setupContextMenu();

    if (sink) 
    {
         jumpToPage(currpage, true);
    }
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
        actionPrint->setEnabled(x);

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
	actionNextFrame->setEnabled(f);
	actionPreviousFrame->setEnabled(f);

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

void ComicMainWindow::toggleScrollbars(bool f)
{
    cfg->scrollbarsVisible(f);
    view->enableScrollbars(f);
}

void ComicMainWindow::toggleContinousView(bool f)
{
    cfg->continuousScrolling(f);
    setupComicImageView();
}

void ComicMainWindow::toggleTwoPages(bool f)
{
    actionMangaMode->setDisabled(!f);
    cfg->twoPagesMode(f);
    view->setTwoPagesMode(f);
}

void ComicMainWindow::toggleJapaneseMode(bool f)
{
    cfg->japaneseMode(f);
    view->setMangaMode(f);
}

void ComicMainWindow::setPageSize(QAction *action)
{
    Size size;
    
    if (action == actionFitWidth)
    {
        size = FitWidth;
    }
    else if (action == actionFitHeight)
    {
        size = FitHeight;
    }
    else if (action == actionBestFit)
    {
        size = BestFit;
    }
    else if (action == actionWholePage)
    {
        size = WholePage;
    }
    else //actionOriginalSize
    {
        size = Original;
    }
    cfg->pageSize(size);
    view->setSize(size);
}

void ComicMainWindow::reloadPage()
{
    if (sink)
    {
        jumpToPage(currpage, true);
    }
}

void ComicMainWindow::updateCaption()
{
    QString c = "QComicBook";
    if (sink)
    {
        c += " - " + sink->getName();
    }
    setWindowTitle(c);
}

void ComicMainWindow::recentSelected(const QString &fname) 
{
    QFileInfo finfo(fname);
    if (!finfo.exists())
    {
        menuRecentFiles->remove(fname);
    }
    open(fname, 0);
}

void ComicMainWindow::pageLoaded(const Page &page)
{
    qDebug() << "page ready " << page.getNumber();
    if (currpage == page.getNumber())
    {
        statusbar->setImageInfo(&page);
    }
}

void ComicMainWindow::pageLoaded(const Page &page1, const Page &page2)
{
    qDebug() << "page ready " << page1.getNumber() << ", " << page2.getNumber();
    if (currpage == page1.getNumber() || currpage == page2.getNumber())
    {
        statusbar->setImageInfo(&page1, &page2);
    }
}

void ComicMainWindow::sinkReady(const QString &path)
{
	statusbar->setShown(actionToggleStatusbar->isChecked() && !(isFullScreen() && cfg->fullScreenHideStatusbar())); //applies back user's statusbar&toolbar preferences
	//toolbar->setShown(actiontoggleToolbar->isOn() && !(isFullScreen() && cfg->fullScreenHideToolbar()));

        menuRecentFiles->add(path);

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
    const int n(view->nextPage(currpage));
    if (n >= 0)
    {
        jumpToPage(n);
    }
}

void ComicMainWindow::prevPage()
{
    const int n(view->previousPage(currpage));
    if (n >= 0)
    {
        jumpToPage(n);
    }
}

void ComicMainWindow::prevFrame()
{
    view->prevFrame();
}

void ComicMainWindow::nextFrame()
{
    view->nextFrame();
}

void ComicMainWindow::prevPageBottom()
{
    if (currpage > 0)
    {
        jumpToPage(view->previousPage(currpage));
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
    {
        jumpToPage(sink->numOfImages() - 1);
    }
}

void ComicMainWindow::forwardPages()
{
    jumpToPage(currpage + 5 );
}

void ComicMainWindow::backwardPages()
{
    jumpToPage(currpage - 5);
}

void ComicMainWindow::jumpToPage(int n, bool force)
{
	if (!sink)
        {
		return;
        }
        if (n >= sink->numOfImages())
        {
                n = sink->numOfImages()-1;
        }
        if (n < 0)
        {
                n = 0;
        }

        if (!actionTogglePreserveRotation->isChecked())
        {
            view->properties().setAngle(None, false);
        }
        if ((n != currpage) || force)
        {
            view->gotoPage(n);
        }
}

void ComicMainWindow::currentPageChanged(int n)
{
    currpage = n;
    const QString page = tr("Page") + " " + QString::number(n + 1) + "/" + QString::number(sink->numOfImages());
    pageinfo->setText(page);
    statusbar->setPage(n + 1, sink->numOfImages());

    //
    // enable or disable next/prev/backward/forward page actions if first/last page shown
    if (sink == NULL || view->nextPage(n) < 0)
    {
        actionNextPage->setDisabled(true);
        actionForwardPage->setDisabled(true);
    }
    else
    {
        actionNextPage->setDisabled(false);
        actionForwardPage->setDisabled(false);
    }
    if (sink == NULL || view->previousPage(n) < 0)
    {
        actionPreviousPage->setDisabled(true);
        actionBackwardPage->setDisabled(true);
    }
    else
    {
        actionPreviousPage->setDisabled(false);
        actionBackwardPage->setDisabled(false);
    }

    thumbswin->view()->scrollToPage(n);
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
    SystemInfoDialog sysinfo(this);
    sysinfo.exec();
}

void ComicMainWindow::showAbout()
{
        AboutDialog *win = new AboutDialog(this, "About QComicBook",
                        "QComicBook " VERSION " - comic book viewer for GNU/Linux<br>"
                        "(c)by Pawel Stolowski 2005-2010<br>"
                        "released under terms of GNU General Public License<br><br>"
                        "<a href=\"http://qcomicbook.linux-projects.net\">http://qcomicbook.linux-projects.net</a><br>"
                        "<a href=\"mailto:stolowski@gmail.com\">stolowski@gmail.com</a>", QPixmap(":/images/qcomicbook-splash.png"));
        win->show();
}

void ComicMainWindow::showConfigDialog()
{
        ComicBookCfgDialog *d = new ComicBookCfgDialog(this, cfg);
        connect(d, SIGNAL(displaySettingsChanged()), this, SLOT(reconfigureDisplay()));
        d->show();
}

void ComicMainWindow::showJumpToPage(const QString &number)
{
        if (sink)
        {
                GoToPageWidget win(this, number.toInt(), sink->numOfImages());
                connect(&win, SIGNAL(pageSelected(int)), this, SLOT(jumpToPage(int)));
                win.exec();
        }
}

void ComicMainWindow::closeSink()
{
    enableComicBookActions(false);

    if (sink)
    {
	frameDetect->clear();
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
		for (int i = 0; i<cnt; i++)
		{
			QString tmpmsg(msg);
			if (cnt > 1)
                        {
				tmpmsg += " (" + tr("page") + " " + QString::number(currpage + i + 1) + ")";
                        }
			QString fname = QFileDialog::getSaveFileName(this, tmpmsg, QString::null, "Images (*.jpg *.png)");
			if (fname.isEmpty())
                        {
                            break;
                        }
                        int result;
                        const Page img = sink->getImage(currpage + i, result);
                        if (result != 0 || !img.getImage().save(fname)) //TODO: overwrite and default format (jpeg)
                        {
                            QMessageBox::critical(this, tr("QComicBook error"), tr("Error saving image"), QMessageBox::Ok, QMessageBox::NoButton);
                            break; //do not attempt to save second image
                        }
		}
	}
}

void ComicMainWindow::openPrintDialog()
{
    QPrintDialog printdlg(printer, this);
    printdlg.setMinMax(1, sink->numOfImages());
    if (printdlg.exec() == QDialog::Accepted)
    {
        PrintProgressDialog *progressdlg = new PrintProgressDialog(this);
        printThread = new PrinterThread(sink, printer, printdlg.printRange(), printdlg.fromPage(), printdlg.toPage());
        connect(printThread, SIGNAL(printing(int)), progressdlg, SLOT(setPage(int)));
        connect(printThread, SIGNAL(finished()), progressdlg, SLOT(close()));
        connect(printThread, SIGNAL(finished()), this, SLOT(printingFinished()));
        connect(progressdlg, SIGNAL(abort()), printThread, SLOT(abort()));
        progressdlg->show();
        printThread->start();
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
			
void ComicMainWindow::changeViewType(QAction *action)
{
    if (action == actionContinuousView)
    {
	cfg->viewType(Continuous);
    }
    else if (action == actionSimpleView)
    {
	cfg->viewType(Simple);
    }
    else // actionFrameView
    {
	cfg->viewType(Frame);
    }
    setupComicImageView();
}

void ComicMainWindow::saveSettings()
{
        cfg->saveGeometry(this);
        cfg->saveDockLayout(this);
        cfg->lastDir(lastdir);
        cfg->recentlyOpened(menuRecentFiles->get());
        
        cfg->showStatusbar(actionToggleStatusbar->isChecked());

        bookmarks->save();        
}

void ComicMainWindow::reconfigureDisplay()
{
    view->setSmallCursor(cfg->smallCursor());
    view->showPageNumbers(cfg->embedPageNumbers());
    view->setBackground(cfg->background());
}

void ComicMainWindow::printingFinished()
{
    printThread->deleteLater();
}
