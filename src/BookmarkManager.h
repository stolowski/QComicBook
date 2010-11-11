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

/*! \file BookmarkManager.h */

#ifndef __BOOKMARKMANAGER_H
#define __BOOKMARKMANAGER_H

#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include "ui_BookmarkManager.h"

class QTreeWidgetItem;

namespace QComicBook
{
	class Bookmarks;
	class Bookmark;

	/**
	 * @brief Widget for managing Bookmarks.
	 */
	class BookmarkManager: public QWidget, private Ui::BookmarkManager
	{
		Q_OBJECT

		private:
			QSharedPointer<Bookmarks> bookmarks;
			QList<QTreeWidgetItem *> invalid; //!<the list of invalid bookmarks

			void initBookmarkView();

		protected slots:
			void selectionChanged();

		public slots:
			void removeSelected();
			void selectInvalid();
			void invertSelection();

		public:
			BookmarkManager(QWidget *parent, QSharedPointer<Bookmarks> b);
			virtual ~BookmarkManager();
	};
}

#endif

