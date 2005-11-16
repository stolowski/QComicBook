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

#include "bookmarkmanager.h"
#include "bookmarks.h"
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <klocale.h>

using namespace QComicBook;

BookmarkManager::BookmarkManager(QWidget *parent, Bookmarks *b): QWidget(parent, NULL, Qt::WType_TopLevel|Qt::WType_Dialog|Qt::WShowModal), bookmarks(b)
{
	QVBoxLayout *box0 = new QVBoxLayout(this, 5, 5);
	lview = new QListView(this);
	lview->addColumn(i18n("Name"));
	lview->addColumn(i18n("Page"));
	lview->setSelectionMode(QListView::Multi);
	box0->addWidget(lview);
	
	QHBoxLayout *box1 = new QHBoxLayout(NULL, 0, 5);

	QPushButton *b_selall = new QPushButton(i18n("Select all"), this);
	box1->addWidget(b_selall);
	box1->addStretch();
	
	QPushButton *b_selrev = new QPushButton(i18n("Invert selection"), this);
	box1->addWidget(b_selrev);
	box1->addStretch(0);
		
	b_selinv = new QPushButton(i18n("Select invalid"), this);
	box1->addWidget(b_selinv);
	box1->addStretch();
	
	QPushButton *b_selnone = new QPushButton(i18n("Clear selection"), this);
	box1->addWidget(b_selnone);
	box1->addStretch(1);
	
	b_remsel = new QPushButton(i18n("Remove selected"), this);
	box1->addWidget(b_remsel);
	box1->addStretch();

	QPushButton *b_ok = new QPushButton(i18n("Close"), this);
	box1->addWidget(b_ok);
	box0->addLayout(box1);

	initBookmarkView();

	connect(lview, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
	connect(b_remsel, SIGNAL(clicked()), this, SLOT(removeSelected()));
	connect(b_selinv, SIGNAL(clicked()), this, SLOT(selectInvalid()));
	connect(b_selall, SIGNAL(clicked()), this, SLOT(selectAll()));
	connect(b_selnone, SIGNAL(clicked()), this, SLOT(selectNone()));
	connect(b_selrev, SIGNAL(clicked()), lview, SLOT(invertSelection()));
	connect(b_ok, SIGNAL(clicked()), this, SLOT(close()));

	selectionChanged();
}

BookmarkManager::~BookmarkManager()
{
}

void BookmarkManager::selectionChanged()
{
	QListViewItemIterator it(lview, QListViewItemIterator::Selected);
	b_remsel->setDisabled(it.current() == 0);
}

void BookmarkManager::initBookmarkView()
{
	lview->clear();
	invalid.clear();
	QValueList<Bookmark> blist = bookmarks->get();
	for (QValueList<Bookmark>::iterator it = blist.begin(); it != blist.end(); it++)
	{
		QListViewItem *item = new QListViewItem(lview);
		item->setText(0, (*it).getName());
		item->setText(1, QString::number((*it).getPage() + 1));

		if (!(*it).isValid())
			invalid.append(item);
	}

	if (invalid.count() == 0)
		b_selinv->setDisabled(true);
}

void BookmarkManager::removeSelected()
{
	QPtrList<QListViewItem> todel;
	for (QListViewItemIterator it(lview, QListViewItemIterator::Selected); it.current(); ++it)
		todel.append(it.current());

	if (todel.count() && QMessageBox::question(this, i18n("Deleting bookmarks"), i18n("Delete selected bookmarks?"),
						   QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		for (QListViewItem *item = todel.first(); item; item = todel.next())
		{
			bookmarks->remove(item->text(0));
			delete item;
		}
		initBookmarkView(); //recreate the view
	}
}

void BookmarkManager::selectAll()
{
	lview->selectAll(true);
}

void BookmarkManager::selectNone()
{
	lview->selectAll(false);
}

void BookmarkManager::selectInvalid()
{
	for (QListViewItem *item = invalid.first(); item; item = invalid.next())
		item->setSelected(true);
}

#include "bookmarkmanager.moc"
