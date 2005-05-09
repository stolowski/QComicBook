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

/*! \file bookmarkmanager.h */

#ifndef __BOOKMARKMANAGER_H
#define __BOOKMARKMANAGER_H

#include <qwidget.h>
#include <qptrlist.h>

class Bookmarks;
class Bookmark;
class QListView;
class QListViewItem;
class QPushButton;

class BookmarkManager: public QWidget
{
	Q_OBJECT

	private:
		QListView *lview;
		QPushButton *b_selinv;
		QPushButton *b_remsel;
		Bookmarks *bookmarks;
		QPtrList<QListViewItem> invalid; //!<the list of invalid bookmarks

		void initBookmarkView();

	protected slots:
		void selectionChanged();

	public slots:
		void removeSelected();
		void selectInvalid();
		void selectAll();
		void selectNone();

	public:
		BookmarkManager(QWidget *parent, Bookmarks *b);
		~BookmarkManager();
};

#endif

