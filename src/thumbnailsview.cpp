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

#include "thumbnailsview.h"
#include "thumbnailitem.h"
#include <qiconview.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qpainter.h>
#include <qptrvector.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <iostream>

#define THUMBNAILS_TMR_DELAY 300

int ThumbnailsView::thwidth = 100;
int ThumbnailsView::thheight = 120;

ThumbnailsView::ThumbnailsView(QWidget *parent): QIconView(parent), selected(NULL)/*{{{*/
{
	setItemsMovable(false);
	arrangeItemsInGrid(true);
	setAutoArrange(true);
	setResizeMode(QIconView::Adjust);
	setMaxItemWidth(thwidth);

	//
	// context menu
	menu = new QPopupMenu(this);
	menu->insertItem(tr("Go to"), this, SLOT(goToPageAction()));
	
	//
	// create "empty page" image
	emptypage = new QPixmap(thwidth, thheight);
	emptypage->fill(Qt::white);
	QPainter paint(emptypage);
	QPen pen(Qt::black, 3);
	paint.setPen(pen);
	paint.drawRect(0, 0, thwidth, thheight);

	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(onContentsMoving(int, int)));
	connect(this, SIGNAL(doubleClicked(QIconViewItem *)), this, SLOT(onDoubleClick(QIconViewItem *)));
	connect(this, SIGNAL(contextMenuRequested(QIconViewItem *, const QPoint&)), this, SLOT(showContextMenu(QIconViewItem *, const QPoint&)));

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(updateVisibleThumbnails()));
}/*}}}*/

ThumbnailsView::~ThumbnailsView()/*{{{*/
{
	delete timer;
	delete emptypage;
}/*}}}*/

void ThumbnailsView::onContentsMoving(int x, int y)/*{{{*/
{
	//
	// find all visible thumbnails
	const QRect rect(x, y, visibleWidth(), visibleHeight());

	ThumbnailItem *it = dynamic_cast<ThumbnailItem *>(findFirstVisibleItem(rect)); 
	const ThumbnailItem *last = dynamic_cast<ThumbnailItem *>(findLastVisibleItem(rect));
	while (it)
	{
		std::cout << it->page() << " ";
		if (!it->isLoaded())
			emit requestedThumbnail(it->page());
		if (it == last)
			break;
		it = dynamic_cast<ThumbnailItem *>(it->nextItem());
	}
	std::cout << std::endl;
}/*}}}*/

void ThumbnailsView::onDoubleClick(QIconViewItem *item)/*{{{*/
{
	emit requestedPage(dynamic_cast<ThumbnailItem *>(item)->page(), false);
}/*}}}*/

void ThumbnailsView::setPages(int pages)/*{{{*/
{
	clear();

	icons.resize(numpages = pages);	
	for (int i=0; i<numpages; i++)
		icons.insert(i, new ThumbnailItem(this, i, *emptypage));

	setArrangement(width() > height() ? QIconView::TopToBottom : QIconView::LeftToRight);

	timer->start(THUMBNAILS_TMR_DELAY, true);
}/*}}}*/

void ThumbnailsView::setPage(int n, const QImage &img)/*{{{*/
{
	ThumbnailItem *th = icons[n];
	th->setPixmap(img);
	th->setLoaded(true);
	repaint(th);
}/*}}}*/

void ThumbnailsView::clear()/*{{{*/
{
	QIconView::clear();
	icons.clear();
	numpages = 0;
	selected = NULL;
}/*}}}*/

void ThumbnailsView::scrollToPage(int n)/*{{{*/
{
	if (n < icons.count())
	{
		ThumbnailItem *th = icons[n];
		setSelected(th, true);
		if (isVisible())
			ensureVisible(th->x(), th->y());
	}
}/*}}}*/

int ThumbnailsView::thumbnailWidth()/*{{{*/
{
	return thwidth;
}/*}}}*/

int ThumbnailsView::thumbnailHeight()/*{{{*/
{
	return thheight;
}/*}}}*/

void ThumbnailsView::updateVisibleThumbnails()/*{{{*/
{
	if (isVisible())
		onContentsMoving(0, 0);
}/*}}}*/

void ThumbnailsView::showContextMenu(QIconViewItem *item, const QPoint &p)/*{{{*/
{
	if (item)
	{
		selected = item;
		menu->popup(p);
	}
}/*}}}*/

void ThumbnailsView::goToPageAction()/*{{{*/
{
	if (selected)
		onDoubleClick(selected);
}/*}}}*/

void ThumbnailsView::adjustItems()/*{{{*/
{
	QIconView::adjustItems();
	updateVisibleThumbnails();
}/*}}}*/

