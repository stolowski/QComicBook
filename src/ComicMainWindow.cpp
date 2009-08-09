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

#include "config.h"
#include "Bookmarks.h"
#include "ComicMainWindow.h"
#include "Icons.h"
#include "cbicons.h"
#include "ComicBookInfo.h"
#include "ComicImageView.h"
#include "ImgArchiveSink.h"
#include "ImgSinkFactory.h"
#include "AboutDialog.h"
#include "ComicBookSettings.h"
#include "History.h"
#include "ComicBookCfgDialog.h"
#include "StatusBar.h"
#include "ThumbnailsWindow.h"
#include "ThumbnailsView.h"
#include "ThumbnailLoader.h"
#include "BookmarkManager.h"
#include "Utility.h"
#include "SupportedArchivesWindow.h"
#include "JumpToPageWindow.h"
#include <QScrollArea>
#include <QImage>
#include <QMenuBar>
#include <QMenu>
#include <QStringList>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QToolBar>
#include <QMessageBox>
#include <QLabel>
#include <QFrame>
#include <QProcess>
#include <QKeyEvent>
#include <QWidgetAction>
#include <QList>
#include <QUrl>
#include <typeinfo>

using namespace QComicBook;
using namespace Utility;

ComicMainWindow::ComicMainWindow(QWidget *parent): QMainWindow(parent), sink(NULL), currpage(0)
{
        updateCaption();
	setAttribute(Qt::WA_DeleteOnClose);
        setWindowIcon(Icons::get(ICON_APPICON));
        setMinimumSize(320, 200);

	setAcceptDrops(true); 

        cfg = &ComicBookSettings::instance();

	cfg->restoreGeometry(this);

        setupThumbnailsWindow();
        setupActions();
        setupToolbar();
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
	addAction(jumpDownAction);
	addAction(jumpUpAction);

        lastdir = cfg->lastDir();
        recentfiles = new History(10);
        *recentfiles = cfg->recentlyOpened();
        setRecentFilesMenu(*recentfiles);

        cfg->restoreDockLayout(this);

        enableComicBookActions(false);
}

ComicMainWindow::~ComicMainWindow()
{
        if (cfg->cacheThumbnails())
                ImgDirSink::removeThumbnails(cfg->thumbnailsAge());

        saveSettings();        

        delete recentfiles;
        delete bookmarks;

        if (sink)
                delete sink;
}

void ComicMainWindow::setupActions()
{
        openArchiveAction = new QAction(Icons::get(ICON_OPENARCH), tr("Open archive"), this);
        openArchiveAction->setShortcut(tr("Ctrl+O"));
        openDirAction = new QAction(Icons::get(ICON_OPENDIR), tr("Open directory"), this);
        openDirAction->setShortcut(tr("Ctrl+D"));
        openNextAction = new QAction(tr("Open next"), this);
        openNextAction->setShortcut(tr("Ctrl+N"));
        openPrevAction = new QAction(tr("Open previous"), this);
        openPrevAction->setShortcut(tr("Ctrl+P"));
	savePageAction = new QAction(tr("Save page as"), this);
        fullScreenAction = new QAction(tr("&Fullscreen"), this);
        fullScreenAction->setShortcut(tr("F11"));
        nextPageAction = new QAction(Icons::get(ICON_NEXTPAGE), tr("Next page"), this);
        nextPageAction->setShortcut(tr("PgDown"));
        forwardPageAction = new QAction(Icons::get(ICON_FORWARD), tr("5 pages forward"), this);
        backwardPageAction = new QAction(Icons::get (ICON_BACKWARD), tr("5 pages backward"), this);
        jumpDownAction = new QAction(QString::null, this);
        jumpDownAction->setShortcut(Qt::Key_Space);
        jumpUpAction = new QAction(QString::null, this);
        jumpUpAction->setShortcut(tr("Backspace"));
        prevPageAction = new QAction(Icons::get(ICON_PREVPAGE), tr("&Previous page"), this);
        prevPageAction->setShortcut(tr("PgUp"));
        pageTopAction = new QAction(Icons::get(ICON_PAGETOP), tr("Page top"), this);
        pageTopAction->setShortcut(tr("Home"));
        pageBottomAction = new QAction(Icons::get(ICON_PAGEBOTTOM), tr("Page bottom"), this);
        pageBottomAction->setShortcut(tr("End"));
        firstPageAction = new QAction(tr("First page"), this);
        firstPageAction->setShortcut(tr("Ctrl+Home"));
        lastPageAction = new QAction(tr("Last page"), this);
        lastPageAction->setShortcut(tr("Ctrl+End"));
        scrollRightAction = new QAction(tr("Scroll right"), this);        
        scrollRightAction->setShortcut(tr("Right"));
        scrollLeftAction = new QAction(tr("Scroll left"), this);
        scrollLeftAction->setShortcut(tr("Left"));
        scrollRightFastAction = new QAction(tr("Fast scroll right"), this);
        scrollRightFastAction->setShortcut(tr("Shift+Right"));
        scrollLeftFastAction = new QAction(tr("Fast scroll left"), this);
        scrollLeftFastAction->setShortcut(tr("Shift+Left"));
        scrollUpAction = new QAction(tr("Scroll up"), this);
        scrollUpAction->setShortcut(tr("Up"));
        scrollDownAction = new QAction(tr("Scroll down"), this);
        scrollDownAction->setShortcut(tr("Down"));
        scrollUpFastAction = new QAction(tr("Fast scroll up"), this);
        scrollUpFastAction->setShortcut(tr("Shift+Up"));
        scrollDownFastAction = new QAction(tr("Fast scroll down"), this);
        scrollDownFastAction->setShortcut(tr("Shift+Down"));
	quitAction = new QAction(tr("Quit"), this);
	quitAction->setShortcut(tr("Ctrl+Q"));

        QActionGroup *scaleActions = new QActionGroup(this);
        fitWidthAction = new QAction(Icons::get(ICON_FITWIDTH), tr("Fit width"), scaleActions);                         
        fitWidthAction->setShortcut(tr("Alt+w"));
        fitWidthAction->setCheckable(true);
        fitHeightAction = new QAction(Icons::get(ICON_FITHEIGHT), tr("Fit height"), scaleActions);
        fitHeightAction->setShortcut(tr("Alt+h"));
        fitHeightAction->setCheckable(true);
        wholePageAction = new QAction(Icons::get(ICON_WHOLEPAGE), tr("Whole page"), scaleActions);
        wholePageAction->setShortcut(tr("Alt+a"));
        wholePageAction->setCheckable(true);
        originalSizeAction = new QAction(Icons::get(ICON_ORGSIZE), tr("Original size"), scaleActions);
        originalSizeAction->setShortcut(tr("Alt+o"));
        originalSizeAction->setCheckable(true);
        bestFitAction = new QAction(Icons::get(ICON_BESTFIT), tr("Best fit"), scaleActions);
        bestFitAction->setShortcut(tr("Alt+b"));
        bestFitAction->setCheckable(true);
        mangaModeAction = new QAction(Icons::get(ICON_JAPANESE), tr("Japanese mode"), this);
        mangaModeAction->setShortcut(tr("Ctrl+j"));
        mangaModeAction->setCheckable(true);
        twoPagesAction = new QAction(Icons::get(ICON_TWOPAGES), tr("Two pages"), this);
        twoPagesAction->setShortcut(tr("Ctrl+t"));
        twoPagesAction->setCheckable(true);
        rotateRightAction = new QAction(Icons::get(ICON_ROTRIGHT), tr("Rotate right"), this);
        rotateLeftAction = new QAction(Icons::get(ICON_ROTLEFT), tr("Rotate left"), this);
        rotateResetAction = new QAction(tr("No rotation"), this);
        togglePreserveRotationAction = new QAction(tr("Preserve rotation"), this);
        togglePreserveRotationAction->setCheckable(true);
        showInfoAction = new QAction(Icons::get(ICON_INFO), tr("Info"), this);
        showInfoAction->setShortcut(tr("Alt+i"));
        exitFullScreenAction = new QAction(QString::null, this);
        exitFullScreenAction->setShortcut(tr("Escape")); //????
        toggleStatusbarAction = new QAction(tr("Statusbar"), this);
        toggleStatusbarAction->setCheckable(true);
        toggleThumbnailsAction = thumbswin->toggleViewAction();
	toggleThumbnailsAction->setIcon(Icons::get(ICON_THUMBNAILS));
        toggleThumbnailsAction->setShortcut(tr("Alt+t"));
        toggleThumbnailsAction->setCheckable(true);

        connect(openArchiveAction, SIGNAL(triggered(bool)), this, SLOT(browseArchive()));
        connect(openDirAction, SIGNAL(triggered(bool)), this, SLOT(browseDirectory()));
        connect(openNextAction, SIGNAL(triggered(bool)), this, SLOT(openNext()));
        connect(openPrevAction, SIGNAL(triggered(bool)), this, SLOT(openPrevious()));
	connect(savePageAction, SIGNAL(triggered(bool)), this, SLOT(savePageAs()));
        connect(showInfoAction, SIGNAL(triggered(bool)), this, SLOT(showInfo()));
        connect(exitFullScreenAction, SIGNAL(triggered(bool)), this, SLOT(exitFullscreen()));
        connect(nextPageAction, SIGNAL(triggered(bool)), this, SLOT(nextPage()));
        connect(forwardPageAction, SIGNAL(triggered(bool)), this, SLOT(forwardPages()));
	connect(firstPageAction, SIGNAL(triggered(bool)), this, SLOT(firstPage()));
	connect(lastPageAction, SIGNAL(triggered(bool)), this, SLOT(lastPage()));
        connect(backwardPageAction, SIGNAL(triggered(bool)), this, SLOT(backwardPages())); 
	connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void ComicMainWindow::setupComicImageView()
{
        view = new ComicImageView(this, cfg->pageSize(), cfg->background());
        setCentralWidget(view);
        view->setFocus();
        view->setSmallCursor(cfg->smallCursor());
        connect(cfg, SIGNAL(backgroundChanged(const QColor&)), view, SLOT(setBackground(const QColor&)));
        connect(cfg, SIGNAL(cursorChanged(bool)), view, SLOT(setSmallCursor(bool)));
        connect(fullScreenAction, SIGNAL(triggered(bool)), this, SLOT(toggleFullScreen()));
        connect(pageTopAction, SIGNAL(triggered(bool)), view, SLOT(scrollToTop()));
        connect(pageBottomAction, SIGNAL(triggered(bool)), view, SLOT(scrollToBottom()));
        connect(scrollRightAction, SIGNAL(triggered(bool)), view, SLOT(scrollRight()));
        connect(scrollLeftAction, SIGNAL(triggered(bool)), view, SLOT(scrollLeft()));
        connect(scrollRightFastAction, SIGNAL(triggered(bool)), view, SLOT(scrollRightFast()));
        connect(scrollLeftFastAction, SIGNAL(triggered(bool)), view, SLOT(scrollLeftFast()));
        connect(scrollUpAction, SIGNAL(triggered(bool)), view, SLOT(scrollUp()));
        connect(scrollDownAction, SIGNAL(triggered(bool)), view, SLOT(scrollDown()));       
        connect(scrollUpFastAction, SIGNAL(triggered(bool)), view, SLOT(scrollUpFast()));        
        connect(scrollDownFastAction, SIGNAL(triggered(bool)), view, SLOT(scrollDownFast()));
        connect(fitWidthAction, SIGNAL(triggered(bool)), view, SLOT(setSizeFitWidth()));        
        connect(fitHeightAction, SIGNAL(triggered(bool)), view, SLOT(setSizeFitHeight()));        
        connect(wholePageAction, SIGNAL(triggered(bool)), view, SLOT(setSizeWholePage()));        
        connect(originalSizeAction, SIGNAL(triggered(bool)), view, SLOT(setSizeOriginal()));        
        connect(bestFitAction, SIGNAL(triggered(bool)), view, SLOT(setSizeBestFit()));        
        connect(mangaModeAction, SIGNAL(toggled(bool)), this, SLOT(toggleJapaneseMode(bool)));        
        connect(twoPagesAction, SIGNAL(toggled(bool)), this, SLOT(toggleTwoPages(bool)));
        connect(prevPageAction, SIGNAL(triggered(bool)), this, SLOT(prevPage()));       
        connect(rotateRightAction, SIGNAL(triggered(bool)), view, SLOT(rotateRight()));        
        connect(rotateLeftAction, SIGNAL(triggered(bool)), view, SLOT(rotateLeft()));
        connect(rotateResetAction, SIGNAL(triggered(bool)), view, SLOT(resetRotation()));
        connect(jumpDownAction, SIGNAL(triggered(bool)), view, SLOT(jumpDown()));
        connect(jumpUpAction, SIGNAL(triggered(bool)), view, SLOT(jumpUp()));
        if (cfg->continuousScrolling())
        {
                connect(view, SIGNAL(bottomReached()), this, SLOT(nextPage()));
                connect(view, SIGNAL(topReached()), this, SLOT(prevPageBottom()));
        }
	connect(view, SIGNAL(doubleClick()), this, SLOT(nextPage()));
        view->enableScrollbars(cfg->scrollbarsVisible());
        QAction *which = originalSizeAction;
        switch (cfg->pageSize())
        {
                case FitWidth:  which = fitWidthAction; break;
                case FitHeight: which = fitHeightAction; break;
                case BestFit:   which = bestFitAction; break;
                case WholePage: which = wholePageAction; break;
                case Original:  which = originalSizeAction; break;
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

void ComicMainWindow::setupToolbar()
{
        toolbar = new QToolBar(tr("Toolbar"), this);
	toolbar->setObjectName("MainToolbar");
        toolbar->addAction(openArchiveAction);
	toolbar->addAction(openDirAction);
        toolbar->addSeparator();
        toolbar->addAction(showInfoAction);
        toolbar->addAction(thumbswin->toggleViewAction());
        toolbar->addSeparator();
        toolbar->addAction(twoPagesAction);
        toolbar->addAction(mangaModeAction);
        toolbar->addSeparator();
        toolbar->addAction(prevPageAction);
        toolbar->addAction(nextPageAction);
        toolbar->addAction(backwardPageAction);
        toolbar->addAction(forwardPageAction);
        toolbar->addAction(pageTopAction);
        toolbar->addAction(pageBottomAction);
        toolbar->addSeparator();
        toolbar->addAction(originalSizeAction);
        toolbar->addAction(fitWidthAction);
        toolbar->addAction(fitHeightAction);
        toolbar->addAction(wholePageAction);
        toolbar->addAction(bestFitAction);
        toolbar->addSeparator();
        toolbar->addAction(rotateRightAction);
        toolbar->addAction(rotateLeftAction);
	addToolBar(toolbar);
}

void ComicMainWindow::setupFileMenu()
{
        file_menu = menuBar()->addMenu(tr("&File"));
        file_menu->addAction(openArchiveAction);
        file_menu->addAction(openDirAction);
        file_menu->addAction(openNextAction);
        file_menu->addAction(openPrevAction);
        recent_menu = file_menu->addMenu(tr("Recently opened"));
        connect(recent_menu, SIGNAL(triggered(QAction *)), this, SLOT(recentSelected(QAction *)));
        file_menu->addSeparator();
	file_menu->addAction(savePageAction);
        file_menu->addSeparator();
	//create_id = file_menu->insertItem(tr("Create archive"), this, SLOT(createArchive()));
        //file_menu->insertSeparator();
       	file_menu->addAction(showInfoAction);
        file_menu->addSeparator();
        closeAction = file_menu->addAction(tr("Close"), this, SLOT(closeSink()));
        file_menu->addSeparator();
	file_menu->addAction(quitAction);
}

void ComicMainWindow::setupViewMenu()
{
        view_menu = menuBar()->addMenu(tr("&View"));
        //view_menu->setCheckable(true); ??? TODO
        view_menu->addAction(originalSizeAction);
        view_menu->addAction(fitWidthAction);
        view_menu->addAction(fitHeightAction);
        view_menu->addAction(wholePageAction);
        view_menu->addAction(bestFitAction);
        view_menu->addSeparator();
        view_menu->addAction(rotateRightAction);
        view_menu->addAction(rotateLeftAction);
        view_menu->addAction(rotateResetAction);
        view_menu->addAction(togglePreserveRotationAction);
        view_menu->addSeparator();
        view_menu->addAction(twoPagesAction);
        view_menu->addAction(mangaModeAction);
        view_menu->addAction(thumbswin->toggleViewAction());
        view_menu->addSeparator();
        view_menu->addAction(fullScreenAction);
}

void ComicMainWindow::setupNavigationMenu()
{
        navi_menu = menuBar()->addMenu(tr("&Navigation"));
        navi_menu->addAction(nextPageAction);
        navi_menu->addAction(prevPageAction);
        navi_menu->addSeparator();
        navi_menu->addAction(forwardPageAction);
        navi_menu->addAction(backwardPageAction);
        navi_menu->addSeparator();
        jumpToAction = navi_menu->addAction(tr("Jump to page..."), this, SLOT(showJumpToPage()));
	navi_menu->addAction(firstPageAction);
	navi_menu->addAction(lastPageAction);
        navi_menu->addSeparator();
        navi_menu->addAction(pageTopAction);
        navi_menu->addAction(pageBottomAction);
        navi_menu->addSeparator();
        contScrollAction = navi_menu->addAction(tr("Continuous scrolling"), this, SLOT(toggleContinousScroll()));
        twoPagesAction->setChecked(cfg->twoPagesMode());
        mangaModeAction->setChecked(cfg->japaneseMode());
        contScrollAction->setChecked(cfg->continuousScrolling());
}

void ComicMainWindow::setupBookmarksMenu()
{
        bookmarks_menu = menuBar()->addMenu(tr("&Bookmarks"));
        bookmarks = new Bookmarks(bookmarks_menu);
        
        setBookmarkAction = bookmarks_menu->addAction(Icons::get(ICON_BOOKMARK), tr("Set bookmark for this comicbook"), this, SLOT(setBookmark()));
        removeBookmarkAction = bookmarks_menu->addAction(tr("Remove bookmark for this comicbook"), this, SLOT(removeBookmark()));
        bookmarks_menu->addAction(tr("Manage bookmarks"), this, SLOT(openBookmarksManager()));
        bookmarks_menu->addSeparator();
        bookmarks->load();
        connect(bookmarks_menu, SIGNAL(triggered(QAction *)), this, SLOT(bookmarkSelected(QAction *)));
}

void ComicMainWindow::setupSettingsMenu()
{
        settings_menu = menuBar()->addMenu(tr("&Settings"));
        toggleScrollbarsAction = settings_menu->addAction(tr("Scrollbars"), this, SLOT(toggleScrollbars()));
	toggleScrollbarsAction->setCheckable(true);
        toggleScrollbarsAction->setChecked(cfg->scrollbarsVisible());
      	settings_menu->addAction(toolbar->toggleViewAction());
        settings_menu->addAction(toggleStatusbarAction);
	settings_menu->addSeparator();
        settings_menu->addAction(Icons::get(ICON_SETTINGS), tr("Configure QComicBook"), this, SLOT(showConfigDialog()));
}

void ComicMainWindow::setupHelpMenu()
{
        QMenu *help_menu = menuBar()->addMenu(tr("&Help"));
	help_menu->addAction(tr("System information"), this, SLOT(showSysInfo()));
	help_menu->addSeparator();
        help_menu->addAction(tr("About"), this, SLOT(showAbout()));
}

void ComicMainWindow::setupStatusbar()
{
        statusbar = new StatusBar(this);      
	setStatusBar(statusbar);
        connect(toggleStatusbarAction, SIGNAL(toggled(bool)), statusbar, SLOT(setVisible(bool)));
        toggleStatusbarAction->setChecked(cfg->showStatusbar());
        statusbar->setShown(cfg->showStatusbar());
}

void ComicMainWindow::setupContextMenu()
{
        QMenu *cmenu = view->contextMenu();
        pageinfo = new QLabel(cmenu);
        pageinfo->setMargin(3);
        pageinfo->setAlignment(Qt::AlignHCenter);
        pageinfo->setFrameStyle(QFrame::Box | QFrame::Raised);
	QWidgetAction *pageinfoAction = new QWidgetAction(this);
	pageinfoAction->setDefaultWidget(pageinfo);

        cmenu->addAction(nextPageAction);
        cmenu->addAction(prevPageAction);
        cmenu->addSeparator();
        cmenu->addAction(fitWidthAction);
        cmenu->addAction(fitHeightAction);
       	cmenu->addAction(wholePageAction);
        cmenu->addAction(originalSizeAction);
        cmenu->addAction(bestFitAction);
        cmenu->addSeparator();
        cmenu->addAction(rotateRightAction);
        cmenu->addAction(rotateLeftAction);
        cmenu->addAction(rotateResetAction);
        cmenu->addSeparator();
        cmenu->addAction(twoPagesAction);
        cmenu->addAction(mangaModeAction);
        cmenu->addSeparator();
        cmenu->addAction(fullScreenAction);
	cmenu->addAction(pageinfoAction);
}

void ComicMainWindow::enableComicBookActions(bool f)
{
        //
        // file menu
        const bool x = f && sink && sink->supportsNext();
        closeAction->setEnabled(f);
        //createAction->setEnabled(f);
        showInfoAction->setEnabled(f);
        openNextAction->setEnabled(x);
        openPrevAction->setEnabled(x);
	savePageAction->setEnabled(x);

        //
        // view menu
        rotateRightAction->setEnabled(f);
        rotateLeftAction->setEnabled(f);
        rotateResetAction->setEnabled(f);

        //
        // navigation menu
	lastPageAction->setEnabled(f);
	firstPageAction->setEnabled(f);
        jumpToAction->setEnabled(f);
        nextPageAction->setEnabled(f);
        prevPageAction->setEnabled(f);
        backwardPageAction->setEnabled(f);
        forwardPageAction->setEnabled(f);
        pageTopAction->setEnabled(f);
        pageBottomAction->setEnabled(f);

        //
        // bookmarks menu
        setBookmarkAction->setEnabled(f);
        removeBookmarkAction->setEnabled(f);
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
                        if (!thumbswin->view()->isLoaded(i))
                                sink->requestThumbnail(i);
        }
}

void ComicMainWindow::toggleScrollbars()
{
        bool f = toggleScrollbarsAction->isChecked();
        //toggleScrollbarsAction->setChecked(!f); //???? potrzebne?
        view->enableScrollbars(f);
}

void ComicMainWindow::toggleContinousScroll()
{
        bool f = contScrollAction->isChecked();
        contScrollAction->setChecked(!f); //???? potrzebne?
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
        twoPagesAction->setChecked(f);
        jumpToPage(currpage, true);
}

void ComicMainWindow::toggleJapaneseMode(bool f)
{
        mangaModeAction->setChecked(f);
        if (twoPagesAction->isChecked())
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
        recent_menu->clear();
        for (QStringList::const_iterator it = list.begin(); it != list.end(); it++)
                recent_menu->addAction(*it);
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
                        recent_menu->removeAction(action);
                }
                open(fname, 0);
        }
}

void ComicMainWindow::sinkReady(const QString &path)
{
	statusbar->setShown(toggleStatusbarAction->isChecked() && !(isFullScreen() && cfg->fullScreenHideStatusbar())); //applies back user's statusbar&toolbar preferences
	//toolbar->setShown(toggleToolbarAction->isOn() && !(isFullScreen() && cfg->fullScreenHideToolbar()));

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

	const bool hasdesc = (sink->getDescription().count() > 0);
	showInfoAction->setDisabled(!hasdesc);

        if (hasdesc && cfg->autoInfo())
                showInfo();
}

void ComicMainWindow::sinkError(int code)
{
	statusbar->setShown(toggleStatusbarAction->isChecked() && !(isFullScreen() && cfg->fullScreenHideStatusbar())); //applies back user's statusbar&toolbar preferences
	//toolbar->setShown(toggleToolbarAction->isOn() && !(isFullScreen() && cfg->fullScreenHideToolbar()));

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
        const QString file = QFileDialog::getOpenFileName(this, tr("Choose a file"), lastdir, "Archives (" + ImgArchiveSink::supportedOpenExtensions().join(" ") + ");;All files (*)");
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
        sink->thumbnailLoader().setReciever(thumbswin);
        sink->thumbnailLoader().setUseCache(cfg->cacheThumbnails());

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
		savedToolbarState = toolbar->toggleViewAction()->isChecked();

                if (cfg->fullScreenHideMenu())
                        menuBar()->hide();
                if (cfg->fullScreenHideStatusbar())
                        statusbar->hide();
                if (cfg->fullScreenHideToolbar())
		{
			toolbar->toggleViewAction()->setChecked(false);
                        toolbar->hide();
		}
                showFullScreen();
        }
}

void ComicMainWindow::exitFullscreen()
{
        if (isFullScreen())
        {
                menuBar()->show();
                if (toggleStatusbarAction->isChecked())
                        statusbar->show();
		toolbar->toggleViewAction()->setChecked(savedToolbarState);
		if (savedToolbarState)
			toolbar->show();
		else
			toolbar->hide();
		showNormal();
        }
}

void ComicMainWindow::nextPage()
{
        if (sink)
        {
                if (twoPagesAction->isChecked() && cfg->twoPagesStep())
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
        jumpToPage(currpage - (twoPagesAction->isChecked() && cfg->twoPagesStep() ? 2 : 1));
}

void ComicMainWindow::prevPageBottom()
{
        if (currpage > 0)
        {
                jumpToPage(currpage - (twoPagesAction->isChecked() && cfg->twoPagesStep() ? 2 : 1));
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
                jumpToPage(sink->numOfImages() - (twoPagesAction->isChecked() && cfg->twoPagesStep() ? 2 : 1));
}

void ComicMainWindow::forwardPages()
{
        jumpToPage(currpage + (twoPagesAction->isChecked() && cfg->twoPagesStep() ? 10 : 5));
}

void ComicMainWindow::backwardPages()
{
        jumpToPage(currpage - (twoPagesAction->isChecked() && cfg->twoPagesStep() ? 10 : 5));
}

void ComicMainWindow::jumpToPage(int n, bool force)
{
	//
	// enable or disable next/prev/backward/forward page actions if first/last page shown
        if (sink == NULL || n == sink->numOfImages() - (twoPagesAction->isChecked() ? 2 : 1))
	{
		nextPageAction->setDisabled(true);
		forwardPageAction->setDisabled(true);
	}
	else
	{
		nextPageAction->setDisabled(false);
		forwardPageAction->setDisabled(false);
	}
        if (sink == NULL || n == (twoPagesAction->isChecked() ? 1 : 0))
	{
		prevPageAction->setDisabled(true);
		backwardPageAction->setDisabled(true);
	}
	else
	{
		prevPageAction->setDisabled(false);
		backwardPageAction->setDisabled(false);
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
                const bool preserveangle = togglePreserveRotationAction->isChecked();

                if (twoPagesAction->isChecked())
                {
                        QImage img1 = sink->getImage(currpage = n, result1);
                        QImage img2 = sink->getImage(currpage + 1, result2);
                        if (result2 == 0)
                        {
                                if (mangaModeAction->isChecked())
                                {
                                        view->setImage(img2, img1, preserveangle);
                                        statusbar->setImageInfo(&img2, &img1);
                                }
                                else
                                {
                                        view->setImage(img1, img2, preserveangle);
                                        statusbar->setImageInfo(&img1, &img2);
                                }
                        }
                        else
                        {
                                view->setImage(img1, preserveangle);
                                statusbar->setImageInfo(&img1);
				if (cfg->preloadPages())
					sink->preload(currpage + 1);
                        }
                }
                else
                {
                        QImage img = sink->getImage(currpage = n, result1);
                        view->setImage(img, preserveangle);
                        statusbar->setImageInfo(&img);
                }
                if (mangaModeAction->isChecked())
                        view->ensureVisible(view->viewWidth(), 0);
		else
			view->ensureVisible(0, 0);
                const QString page = tr("Page") + " " + QString::number(currpage + 1) + "/" + QString::number(sink->numOfImages());
                pageinfo->setText(page);
                statusbar->setPage(currpage + 1, sink->numOfImages());
                thumbswin->view()->scrollToPage(currpage);
		if (cfg->preloadPages())
		{
			if (!twoPagesAction->isChecked())
			{
				sink->preload(currpage + 1);
			}
			else
			{
				sink->preload(currpage + 3);
				sink->preload(currpage + 2);
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

void ComicMainWindow::createArchive()
{
	/*if (sink)
	{
		ArchiverDialog *win = new ArchiverDialog(this, sink);
		win->exec();
		delete win;
	}*/
}

void ComicMainWindow::showAbout()
{
        AboutDialog *win = new AboutDialog(this, "About QComicBook",
                        "QComicBook " VERSION " - comic book viewer for GNU/Linux\n"
                        "(c)by Pawel Stolowski 2005-2007\n"
                        "released under terms of GNU General Public License\n\n"
                        "http://linux.bydg.org/~yogin\n"
                        "pawel.stolowski@wp.pl", QPixmap(DATADIR "qcomicbook-splash.png"));
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
				QImage img = sink->getImage(currpage, result);
				if (result != 0 || !img.save(fname)) //TODO: overwrite and default format (jpeg)
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
        cfg->scrollbarsVisible(toggleScrollbarsAction->isChecked());
        cfg->twoPagesMode(twoPagesAction->isChecked());
        cfg->japaneseMode(mangaModeAction->isChecked());
        cfg->continuousScrolling(contScrollAction->isChecked());
        cfg->lastDir(lastdir);
        cfg->recentlyOpened(*recentfiles);
        cfg->pageSize(view->getSize());
        cfg->showStatusbar(toggleStatusbarAction->isChecked());

        bookmarks->save();        
}

