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
#include <iostream>
#include "imgarchivesink.h"
#include "icons.h"
#include "comicmain.h"
#include "cbsettings.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	if (!Icons::init(DATADIR))
		std::cerr << "can't initialize icons path:" << DATADIR << std::endl;

	if (!ComicBookSettings::checkDirs())
		std::cerr << "can't initialize qcomicbook directories" << std::endl;
	
	//
	// initialize unpackers
	bool rar = ImgArchiveSink::autoconfRAR();
	bool zip = ImgArchiveSink::autoconfZIP();
	QString msg;

	if (!rar)
		msg = "RAR packer/unpacker not found";
	if (!zip)
	{
		if (!rar)
			msg += "\n";
		msg += "ZIP packer/unpacker not found";
	}

	if (!(rar && zip))
	{
		QMessageBox::critical(NULL, "QComicBook error", msg, QMessageBox::Ok, QMessageBox::NoButton);
	}
	
	ComicMainWindow *win = new ComicMainWindow(NULL);
	app.setMainWidget(win);
	win->show();

	//
	// command line argument
	if (app.argc() > 1)
		win->open(app.argv()[1]);

	return app.exec();
}

