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

#include <qapplication.h>
#include <qmessagebox.h>
#include <qsplashscreen.h>
#include <qtimer.h>
#include "imgarchivesink.h"
#include "icons.h"
#include "comicmain.h"
#include "cbsettings.h"

int main(int argc, char *argv[])
{
	using namespace QComicBook;

	QApplication app(argc, argv);
	const QString errcaption = ComicMainWindow::tr("QComicBook error");

	ComicBookSettings::instance().load();

	//
	// show splashscreen
	QSplashScreen *splash = NULL;
	if (ComicBookSettings::instance().showSplash())
	{
		QPixmap splashpix(DATADIR "qcomicbook-splash.png");
		if (!splashpix.isNull())
		{
			splash = new QSplashScreen(splashpix);
			splash->show();
		}
	}
	
	if (!Icons::init(DATADIR))
		QMessageBox::critical(NULL, errcaption, ComicMainWindow::tr("Can't initialize icons path") + ":\n" DATADIR,
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
		//splash->finish(win);
	}

	return app.exec();
}

