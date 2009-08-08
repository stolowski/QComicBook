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

#include "SupportedArchivesWindow.h"
#include "ImgArchiveSink.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLayout>
#include <QDialogButtonBox>

using namespace QComicBook;

SupportedArchivesWindow::SupportedArchivesWindow(QWidget *parent): QDialog(parent)
{
	setWindowTitle(tr("QComicBook System Information"));
	setWindowModality(Qt::WindowModal);
	resize(560, 400);
	QVBoxLayout *lay0 = new QVBoxLayout(this);

	QTreeWidget *list = new QTreeWidget(this);
	list->setColumnCount(2);
	list->setItemHidden(list->headerItem(), true);
	list->setRootIsDecorated(true);
	lay0->addWidget(list);

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
	//QTreeWidgetItem *tools = new QTreeWidgetItem(list, QStringList(tr("External tools")));

	list->expandAll();

	list->resizeColumnToContents(0);
	list->resizeColumnToContents(1);

	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
	lay0->addWidget(buttons);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
}

SupportedArchivesWindow::~SupportedArchivesWindow()
{
}

