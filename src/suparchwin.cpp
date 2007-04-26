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
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLayout>
#include <QPushButton>
#include <QLabel>

using namespace QComicBook;

SupportedArchivesWindow::SupportedArchivesWindow(QWidget *parent): QWidget(parent, Qt::Dialog) //|Qt::WDestructiveClose)
{
	setWindowTitle("QComicBook System Information");
	setWindowModality(Qt::WindowModal);
	resize(560, 400);
	QVBoxLayout *lay0 = new QVBoxLayout(this);

	QTreeWidget *list = new QTreeWidget(this);
	//list->header()->hide();
	//list->setRootIsDecorated(true);
	lay0->addWidget(list);
	//list->addColumn(tr("Archive Type"));
	//list->addColumn(tr("Support"));

	const QString yes = tr("Yes");
	const QString no = tr("No");
	
	QTreeWidgetItem *arch = new QTreeWidgetItem(list, QStringList(tr("Archivers")));
	const QList<ImgArchiveSink::ArchiveTypeInfo> ainf = ImgArchiveSink::supportedArchivesInfo();
	for (QList<ImgArchiveSink::ArchiveTypeInfo>::const_iterator it = ainf.begin(); it!=ainf.end(); it++)
	{
		const ImgArchiveSink::ArchiveTypeInfo &inf = *it;
		QTreeWidgetItem *aitem = new QTreeWidgetItem(arch, QStringList(inf.name));
		QStringList opsr(tr("reading"));
		QStringList opsw(tr("writing"));
		opsr << (inf.reading ? yes : no);
		opsw << (inf.writing ? yes : no);
		new QTreeWidgetItem(aitem, opsr);
		new QTreeWidgetItem(aitem, opsw);
		aitem->setExpanded(true);
	}
	QTreeWidgetItem *tools = new QTreeWidgetItem(list, QStringList(tr("External tools")));
	arch->setExpanded(true);
	tools->setExpanded(true);

	QPushButton *b_close = new QPushButton(tr("Close"), this);
	connect(b_close, SIGNAL(clicked()), this, SLOT(close()));
	lay0->addWidget(b_close);
}

SupportedArchivesWindow::~SupportedArchivesWindow()
{
}

