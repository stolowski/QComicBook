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

#include "thumbnailsview.h"
#include "iconviewthumbnail.h"
#include "thumbnail.h"
#include <qiconview.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qpainter.h>
#include <qptrvector.h>
#include <qpopupmenu.h>

using namespace QComicBook;

ThumbnailsView::ThumbnailsView(QWidget *parent): QListWidget(parent), selected(NULL), numpages(0)
{
	setFocusPolicy(QWidget::NoFocus);
	setItemsMovable(false);
	arrangeItemsInGrid(true);
	setAutoArrange(true);
	setResizeMode(QIconView::Adjust);
	setMaxItemWidth(Thumbnail::maxWidth());

	//
	// context menu
	menu = new QPopupMenu(this);
	menu->insertItem(tr("Go to"), this, SLOT(goToPageAction()));
	
	//
	// create "empty page" image
	emptypage = new QPixmap(Thumbnail::maxWidth(), Thumbnail::maxHeight());
	emptypage->fill(Qt::white);
	QPainter paint(emptypage);
	QPen pen(Qt::black, 3);
	paint.setPen(pen);
	paint.drawRect(0, 0, Thumbnail::maxWidth(), Thumbnail::maxHeight());

	connect(this, SIGNAL(doubleClicked(QIconViewItem *)), this, SLOT(onDoubleClick(QIconViewItem *)));
	connect(this, SIGNAL(contextMenuRequested(QIconViewItem *, const QPoint&)), this, SLOT(showContextMenu(QIconViewItem *, const QPoint&)));
}

ThumbnailsView::~ThumbnailsView()
{
	delete emptypage;
}

void ThumbnailsView::onDoubleClick(QIconViewItem *item)
{
	emit requestedPage(dynamic_cast<ThumbnailItem *>(item)->page(), false);
}

void ThumbnailsView::setPages(int pages)
{
	clear();

	icons.resize(numpages = pages);	
	for (int i=0; i<numpages; i++)
		icons.insert(i, new IconViewThumbnail(this, i, *emptypage));

	//setArrangement(visibleWidth() > visibleHeight() ? QIconView::LeftToRight : QIconView::TopToBottom);
}

void ThumbnailsView::setPage(int n, const QImage &img)
{
	if (n < icons.count())
	{
		IconViewThumbnail *th = icons[n];
		th->setPixmap(img);
		th->setLoaded(true);
	}
}

void ThumbnailsView::setPage(const Thumbnail &t)
{
	setPage(t.page(), t.image());
}

void ThumbnailsView::clear()
{
	QIconView::clear();
	icons.clear();
	numpages = 0;
	selected = NULL;
}

void ThumbnailsView::scrollToPage(int n)
{
	if (n < icons.count())
	{
		IconViewThumbnail *th = icons[n];
		setSelected(th, true);
		if (isVisible())
			ensureVisible(th->x(), th->y());
	}
}

void ThumbnailsView::showContextMenu(QIconViewItem *item, const QPoint &p)
{
	if (item)
	{
		selected = item;
		menu->popup(p);
	}
}

void ThumbnailsView::goToPageAction()
{
	if (selected)
		onDoubleClick(selected);
}

bool ThumbnailsView::isLoaded(int n) const
{
	return (n < icons.count()) ? icons[n]->isLoaded() : false;
}

