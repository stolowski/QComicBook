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
class QTimer;
class QPopupMenu;

class ThumbnailsView: public QIconView
{
	Q_OBJECT

	private:
		int numpages;
		QPixmap *emptypage;
		QPtrVector<ThumbnailItem> icons;
		QTimer *timer;
		QPopupMenu *menu;
		QIconViewItem *selected;
		static int thwidth, thheight; //default thumbnail width and height

	signals:
		void requestedThumbnail(int n);
		void requestedPage(int n, bool force);

	protected slots:
		void onContentsMoving(int x, int y);
		void onDoubleClick(QIconViewItem *item);
		void showContextMenu(QIconViewItem *item, const QPoint &p);
		void goToPageAction();
		void adjustItems();

	public:
		ThumbnailsView(QWidget *parent);
		virtual ~ThumbnailsView();

		static int thumbnailWidth();
		static int thumbnailHeight();

	public slots:
		void setPages(int pages);
		void setPage(int n, const QImage &img);
		void clear();
		void scrollToPage(int n);
		void updateVisibleThumbnails();
};

#endif

