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

#ifndef __PAGESDIRECTOR_H
#define __PAGESDIRECTOR_H

/*! \file pagesdirector.h */

#include <QListView>
#include <QList>

namespace QComicBook
{
	class ImgDirSink;
	class ThumbnailLoaderThread;

	class PagesDirector: public QListView
	{
		Q_OBJECT

		private:
			ThumbnailLoaderThread *loader;
			QCheckListItem **pages;
			ImgDirSink *sink;

		protected:
			virtual void customEvent(QCustomEvent *e);

		public slots:
			void moveUp(QListViewItem *item);
			void moveDown(QListViewItem *item);
			void moveUpSelected();
			void moveDownSelected();
			void makeSelectedFirst(QListViewItem *item);
			void makeSelectedLast(QListViewItem *item);
			void makeSelectedFirst();
			void makeSelectedLast();
				
		public:
			PagesDirector(QWidget *parent, ImgDirSink *snk);
			virtual ~PagesDirector();

			QList<int> markedPages();
	};
}

#endif

