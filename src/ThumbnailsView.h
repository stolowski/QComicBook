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

#ifndef __THUMBNAILSVIEW_H
#define __THUMBNAILSVIEW_H

#include <QListWidget>
#include <QVector>
#include <iconviewthumbnail.h>

class QPixmap;
class QMenu;
class QListWidgetItem;
 
namespace QComicBook
{
	class Thumbnail;

	class ThumbnailsView: public QListWidget
	{
		Q_OBJECT

		private:
			int numpages;
			QPixmap *emptypage;
			QVector<IconViewThumbnail *> icons;
			QMenu *menu;
			QListWidgetItem *selected;

		signals:
			void requestedPage(int n, bool force);

		protected slots:
			void onDoubleClick(QListWidgetItem *item);
			void goToPageAction();
			virtual void contextMenuEvent(QContextMenuEvent *e);

		public:
			ThumbnailsView(QWidget *parent);
			virtual ~ThumbnailsView();
			virtual bool isLoaded(int n) const;

		public slots:
			void setPages(int pages);
			void setPage(int n, const QPixmap &img);
			void setPage(const Thumbnail &t);
			void clear();
			void scrollToPage(int n);
	};
}

#endif

