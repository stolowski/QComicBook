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

#include "helpbrowser.h"
#include <qtextbrowser.h>
#include <qtextcodec.h>
#include <qfileinfo.h>
#include <qpopupmenu.h>
#include <qmenubar.h>

HelpBrowser::HelpBrowser(const QString &caption, const QString &path, const QString &file, const QString &imgpath, QWidget *parent): QMainWindow(parent)
{
	const QString locale = QTextCodec::locale();
	const QString paths[] = { path + "/" + locale,
		                  path + "/" + locale.left(4),
				  path + "/" + locale.left(2),
				  path + "/en" };

	setCaption(caption);
	txtb = new QTextBrowser(this);
	setCentralWidget(txtb);

	//
	// try to find translated help first according to locale setting; fallback to en
	for (int i=0; i<4; i++)
	{
		QFileInfo f(paths[i] + "/" + file);
		if (f.exists())
		{
			QStringList fpaths;
			fpaths.append(paths[i]);
			if (!imgpath.isEmpty())
				fpaths.append(path + "/" + imgpath); //append common images subdirectory
			txtb->mimeSourceFactory()->setFilePath(fpaths);
			txtb->setSource(file);
			break;
		}
	}

	//
	// menu
	QPopupMenu *file_menu = new QPopupMenu(this);
	file_menu->insertItem(tr("Quit"), this, SLOT(close()));
	menuBar()->insertItem(tr("File"), file_menu);
	go_menu = new QPopupMenu(this);
	go_menu->insertItem(tr("Table of contents"), txtb, SLOT(home()));
	go_menu->insertSeparator();
	id_back = go_menu->insertItem(tr("Back"), txtb, SLOT(backward()));
	id_forward = go_menu->insertItem(tr("Forward"), txtb, SLOT(forward()));
	menuBar()->insertItem(tr("Go"), go_menu);

	enableBackward(false);
	enableForward(false);
	
	connect(txtb, SIGNAL(backwardAvailable(bool)), this, SLOT(enableBackward(bool)));
	connect(txtb, SIGNAL(forwardAvailable(bool)), this, SLOT(enableForward(bool)));

	resize(640, 480);
}

HelpBrowser::~HelpBrowser()
{
}

void HelpBrowser::enableBackward(bool f)
{
	go_menu->setItemEnabled(id_back, f);
}

void HelpBrowser::enableForward(bool f)
{
	go_menu->setItemEnabled(id_forward, f);
}

