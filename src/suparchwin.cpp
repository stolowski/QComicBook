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

#include "suparchwin.h"
#include "imgarchivesink.h"
#include <qlistview.h>
#include <qheader.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

using namespace QComicBook;

SupportedArchivesWindow::SupportedArchivesWindow(QWidget *parent): QWidget(parent, 0, Qt::WType_Dialog|Qt::WShowModal|Qt::WDestructiveClose)
{
	setCaption("QComicBook System Information");
	resize(560, 400);
	QVBoxLayout *lay0 = new QVBoxLayout(this, 5, 5);

	QListView *list = new QListView(this);
	list->header()->hide();
	list->setRootIsDecorated(true);
	lay0->add(list);
	list->addColumn(tr("Archive Type"));
	list->addColumn(tr("Support"));

	const QString yes = tr("Yes");
	const QString no = tr("No");
	
	QListViewItem *arch = new QListViewItem(list, tr("Archivers"));
	const QValueList<ImgArchiveSink::ArchiveTypeInfo> ainf = ImgArchiveSink::supportedArchivesInfo();
	for (QValueList<ImgArchiveSink::ArchiveTypeInfo>::const_iterator it = ainf.begin(); it!=ainf.end(); it++)
	{
		const ImgArchiveSink::ArchiveTypeInfo &inf = *it;
		QListViewItem *aitem = new QListViewItem(arch, inf.name);
		new QListViewItem(aitem, tr("reading"), inf.reading ? yes : no);
		new QListViewItem(aitem, tr("writing"), inf.writing ? yes : no);
		aitem->setOpen(true);
	}
	QListViewItem *tools = new QListViewItem(list, tr("External tools"));
	list->setOpen(arch, true);
	list->setOpen(tools, true);

	QPushButton *b_close = new QPushButton(tr("Close"), this);
	connect(b_close, SIGNAL(clicked()), this, SLOT(close()));
	lay0->addWidget(b_close);
}

SupportedArchivesWindow::~SupportedArchivesWindow()
{
}

