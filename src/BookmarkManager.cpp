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

BookmarkManager::BookmarkManager(QWidget *parent, Bookmarks *b): QWidget(parent, Qt::Dialog), bookmarks(b)
{
    setWindowTitle(tr("QComicBook Bookmarks"));
    setWindowModality(Qt::WindowModal);
    
    QVBoxLayout *box0 = new QVBoxLayout(this);
    lview = new QTreeWidget(this);
    QStringList labels;
    labels << tr("Name") << tr("Page");
    lview->setHeaderLabels(labels);
    lview->setSelectionMode(QAbstractItemView::MultiSelection);
    box0->addWidget(lview);
	
    QHBoxLayout *box1 = new QHBoxLayout(NULL);
    
    QPushButton *b_selall = new QPushButton(tr("Select all"), this);
    box1->addWidget(b_selall);
    box1->addStretch();
    
    QPushButton *b_selrev = new QPushButton(tr("Invert selection"), this);
    box1->addWidget(b_selrev);
    box1->addStretch(0);
		
    b_selinv = new QPushButton(tr("Select invalid"), this);
    box1->addWidget(b_selinv);
    box1->addStretch();
    
    QPushButton *b_selnone = new QPushButton(tr("Clear selection"), this);
    box1->addWidget(b_selnone);
    box1->addStretch(1);
    
    b_remsel = new QPushButton(tr("Remove selected"), this);
    box1->addWidget(b_remsel);
    box1->addStretch();
    
    QPushButton *b_ok = new QPushButton(tr("Close"), this);
    box1->addWidget(b_ok);
    box0->addLayout(box1);
    
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

