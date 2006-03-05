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

#ifndef __PAGESDIRECTOR_H
#define __PAGESDIRECTOR_H

/*! \file pagesdirector.h */

#include <qlistview.h>
#include <qvaluelist.h>

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
			virtual void moveUp(QListViewItem *item);
			virtual void moveDown(QListViewItem *item);
			virtual void moveUpSelected();
			virtual void moveDownSelected();
				
		public:
			PagesDirector(QWidget *parent, ImgDirSink *snk);
			virtual ~PagesDirector();

			QValueList<int> markedPages();
	};
}

#endif

