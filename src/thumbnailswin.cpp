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

#include "thumbnailswin.h"
#include "thumbnailsview.h"

ThumbnailsWindow::ThumbnailsWindow(Place p, QWidget *parent): QDockWindow(p, parent)/*{{{*/
{
	setCaption(tr("Thumbnails"));
	setResizeEnabled(true);
	setCloseMode(QDockWindow::Always);

	view = new ThumbnailsView(this);
	setWidget(view);
	connect(view, SIGNAL(requestedThumbnail(int)), this, SIGNAL(requestedThumbnail(int)));
	connect(view, SIGNAL(requestedPage(int, bool)), this, SIGNAL(requestedPage(int, bool)));
	connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(onVisibilityChanged(bool)));
	connect(this, SIGNAL(orientationChanged(Orientation)), this, SLOT(onOrientationChanged(Orientation)));
}/*}}}*/

ThumbnailsWindow::~ThumbnailsWindow()/*{{{*/
{
}/*}}}*/

void ThumbnailsWindow::setPages(int pages)/*{{{*/
{
	view->setPages(pages);
}/*}}}*/

void ThumbnailsWindow::setPage(int n, const QImage &img)/*{{{*/
{
	view->setPage(n, img);
}/*}}}*/

void ThumbnailsWindow::clear()/*{{{*/
{
	view->clear();
}/*}}}*/

void ThumbnailsWindow::scrollToPage(int n)/*{{{*/
{
	view->scrollToPage(n);
}/*}}}*/

void ThumbnailsWindow::onVisibilityChanged(bool f)/*{{{*/
{
	if (f)
		view->updateVisibleThumbnails();
}/*}}}*/

void ThumbnailsWindow::onOrientationChanged(Orientation o)/*{{{*/
{
	view->setArrangement(o == Qt::Horizontal ? QIconView::TopToBottom : QIconView::LeftToRight);
}/*}}}*/

