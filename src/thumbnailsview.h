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

#ifndef __THUMBNAILSVIEW_H
#define __THUMBNAILSVIEW_H

#include <qiconview.h>
#include <qptrvector.h>
#include <thumbnailitem.h>

class QPixmap;
class QIconViewItem;
class QPopupMenu;
class Thumbnail;

class ThumbnailsView: public QIconView
{
	Q_OBJECT

	private:
		int numpages;
		QPixmap *emptypage;
		QPtrVector<ThumbnailItem> icons;
		QPopupMenu *menu;
		QIconViewItem *selected;

	signals:
		void requestedPage(int n, bool force);

	protected slots:
		void onDoubleClick(QIconViewItem *item);
		void showContextMenu(QIconViewItem *item, const QPoint &p);
		void goToPageAction();

	public:
		ThumbnailsView(QWidget *parent);
		virtual ~ThumbnailsView();
		
	public slots:
		void setPages(int pages);
		void setPage(int n, const QImage &img);
		void setPage(const Thumbnail &t);
		void clear();
		void scrollToPage(int n);
};

#endif

