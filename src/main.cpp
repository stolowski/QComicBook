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

#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTimer>
#include "ComicMainWindow.h"
#include "ComicBookSettings.h"
#include "config.h"

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
		QPixmap splashpix(":/images/qcomicbook-splash.png");
		if (!splashpix.isNull())
		{
			splash = new QSplashScreen(splashpix);
			splash->show();
		}
	}
	
	if (!ComicBookSettings::checkDirs())
		QMessageBox::critical(NULL, errcaption, ComicMainWindow::tr("Can't initialize QComicBook directories"),
				QMessageBox::Ok, QMessageBox::NoButton);
	
	ComicMainWindow *win = new ComicMainWindow(NULL);
	//app.setMainWidget(win);
	win->show();

	//
	// command line argument
	if (app.argc() > 1)
		win->open(QString::fromLocal8Bit(app.argv()[1]));

	//
	// close splashscreen after a few seconds
	if (splash)
	{
		QTimer *timer = new QTimer(win);
		QObject::connect(timer, SIGNAL(timeout()), splash, SLOT(close()));
		timer->setSingleShot(true);
		timer->start(2000);
		//splash->finish(win);
	}

	return app.exec();
}

