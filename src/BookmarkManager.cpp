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

#include "BookmarkManager.h"
#include "Bookmarks.h"
#include <Qt>
#include <QTreeWidget>
#include <QPushButton>
#include <QLayout>
#include <QMessageBox>

using namespace QComicBook;

BookmarkManager::BookmarkManager(QWidget *parent, QSharedPointer<Bookmarks> b): QWidget(parent, Qt::Dialog), bookmarks(b)
{
    setupUi(this);

    initBookmarkView();
    
    connect(lview, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    connect(b_remsel, SIGNAL(clicked()), this, SLOT(removeSelected()));
    connect(b_selinv, SIGNAL(clicked()), this, SLOT(selectInvalid()));
    connect(b_selall, SIGNAL(clicked()), lview, SLOT(selectAll()));
    connect(b_selnone, SIGNAL(clicked()), lview, SLOT(clearSelection()));
    connect(b_selrev, SIGNAL(clicked()), this, SLOT(invertSelection()));
    connect(b_ok, SIGNAL(clicked()), this, SLOT(close()));

    selectionChanged();
}

BookmarkManager::~BookmarkManager()
{
}

void BookmarkManager::selectionChanged()
{
	b_remsel->setDisabled(lview->selectedItems().size() == 0);
}

void BookmarkManager::initBookmarkView()
{
	lview->clear();
	invalid.clear();
	QList<Bookmark> blist = bookmarks->get();
	foreach (Bookmark bk, blist) 
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(lview);
		item->setText(0, bk.getName());
		item->setText(1, QString::number(bk.getPage() + 1));

		if (!bk.isValid())
			invalid.append(item);
	}

	lview->resizeColumnToContents(0); // make sure entire path is visible

	if (invalid.count() == 0)
		b_selinv->setDisabled(true);
}

void BookmarkManager::invertSelection()
{
	for (QTreeWidgetItemIterator it(lview); *it; ++it)
	{
		(*it)->setSelected(! (*it)->isSelected());
	}
}

void BookmarkManager::removeSelected()
{
	QList<QTreeWidgetItem *> todel(lview->selectedItems());
	if (todel.count() && QMessageBox::question(this, tr("Deleting bookmarks"), tr("Delete selected bookmarks?"),
						   QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		foreach (QTreeWidgetItem *item, todel)
		{
			bookmarks->remove(item->text(0));
			delete item;
		}
		initBookmarkView(); //recreate the view
	}
}

void BookmarkManager::selectInvalid()
{
	foreach (QTreeWidgetItem *item, invalid)
		item->setSelected(true);
}

