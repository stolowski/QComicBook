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
#include "imgarchivesink.h"
#include "icons.h"
#include "comicmain.h"
#include "cbsettings.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	const QString errcaption = ComicMainWindow::tr("QComicBook error");

	ComicBookSettings::instance().load();
	
	if (!Icons::init(DATADIR))
		QMessageBox::critical(NULL, errcaption, ComicMainWindow::tr("Can't initialize icons path") + ":\n" DATADIR,
				QMessageBox::Ok, QMessageBox::NoButton);

	if (!ComicBookSettings::checkDirs())
		QMessageBox::critical(NULL, errcaption, ComicMainWindow::tr("Can't initialize QComicBook directories"),
				QMessageBox::Ok, QMessageBox::NoButton);
	
	//
	// initialize unpackers
	ImgArchiveSink::autoconfRAR();
	ImgArchiveSink::autoconfZIP();
	ImgArchiveSink::autoconfACE();
	ImgArchiveSink::autoconfTARGZ();
	ImgArchiveSink::autoconfTARBZ2();
	
	ComicMainWindow *win = new ComicMainWindow(NULL);
	app.setMainWidget(win);
	win->show();

	//
	// command line argument
	if (app.argc() > 1)
		win->open(app.argv()[1]);

	return app.exec();
}

