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

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <qmessagebox.h>
#include <ksplashscreen.h>
#include <qtimer.h>
#include <kstandarddirs.h>
#include "imgarchivesink.h"
#include "icons.h"
#include "comicmain.h"
#include "cbsettings.h"

static const char description[] =
    I18N_NOOP("Disk cleaning tool");

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char *argv[])
{
	using namespace QComicBook;

	KAboutData about("qcomicbook", I18N_NOOP("QComicBook"), VERSION, description,
                     KAboutData::License_GPL, "(C) 2005 Pawel Stolowski", 0, "http://linux.bydg.org/~yogin", "pawel.stolowski@wp.pl");
	about.addAuthor( "Pawel Stolowski", 0, "pawel.stolowski@wp.pl" );
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions( options );

	KApplication app;
	const QString errcaption = ComicMainWindow::tr("QComicBook error");

	//
	// show splashscreen
	QPixmap splashpix(locate("appdata", "qcomicbook-splash.png"));
	KSplashScreen *splash = NULL;
	if (!splashpix.isNull())
	{
		splash = new KSplashScreen(splashpix);
		splash->show();
	}

	ComicBookSettings::instance().load();
	
	if (!Icons::init())
		QMessageBox::critical(NULL, errcaption, ComicMainWindow::tr("Can't find icons"),
				QMessageBox::Ok, QMessageBox::NoButton);

	if (!ComicBookSettings::checkDirs())
		QMessageBox::critical(NULL, errcaption, ComicMainWindow::tr("Can't initialize QComicBook directories"),
				QMessageBox::Ok, QMessageBox::NoButton);
	
	//
	// initialize unpackers
	ImgArchiveSink::autoconfArchivers();
		
	ComicMainWindow *win = new ComicMainWindow(NULL);
	app.setMainWidget(win);
	win->show();

	//
	// command line argument
	if (app.argc() > 1)
		win->open(app.argv()[1]);

	//
	// close splashscreen after a few seconds
	if (splash)
	{
		QTimer *timer = new QTimer(win);
		QObject::connect(timer, SIGNAL(timeout()), splash, SLOT(close()));
		timer->start(2000, true);
	}

	return app.exec();
}

