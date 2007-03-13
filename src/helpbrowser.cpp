/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <yogin@linux.bydg.org>
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
#include <qmenubar.h>

using namespace QComicBook;

HelpBrowser::HelpBrowser(const QString &caption, const QString &path, const QString &file, const QString &imgpath, QWidget *parent): QMainWindow(parent)
{
	/*setWindowTitle(caption);
	txtb = new QTextBrowser(this);
	setCentralWidget(txtb);

	QStringList fpaths;
	fpaths.append(path);
	//if (!imgpath.isEmpty())
	//	fpaths.append(path + "/" + imgpath); //append common images subdirectory
	txtb->mimeSourceFactory()->setFilePath(fpaths);
	txtb->setSource(file);

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

	resize(640, 480);*/
}

HelpBrowser::~HelpBrowser()
{
}

void HelpBrowser::enableBackward(bool f)
{
	//go_menu->setItemEnabled(id_back, f);
}

void HelpBrowser::enableForward(bool f)
{
	//go_menu->setItemEnabled(id_forward, f);
}

QString HelpBrowser::getLocaleHelpDir(const QString &maindir, const QString &file)
{
	/*const QString locale = QTextCodec::locale();
	const QString paths[] = { maindir + "/" + locale,
		                  maindir + "/" + locale.left(4),
				  maindir + "/" + locale.left(2),
				  maindir + "/en" };
	//
	// try to find translated help first according to locale setting; fallback to en
	for (int i=0; i<4; i++)
	{
		QFileInfo f(paths[i] + "/" + file);
		if (f.exists())
			return f.absFilePath();
	}
	return QString::null;*/
}

