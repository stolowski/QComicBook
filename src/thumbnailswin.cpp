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

#include "thumbnailswin.h"
#include "thumbnailsview.h"
#include "thumbnailloader.h"
#include "thumbnail.h"
#include "thumbnailevent.h"

using namespace QComicBook;

ThumbnailsWindow::ThumbnailsWindow(QWidget *parent): QDockWidget(parent)
{
	setWindowTitle(tr("Thumbnails"));
	//setResizeEnabled(true);
	//setCloseMode(QDockWindow::Always);

	tview = new ThumbnailsView(this);
	setWidget(tview);
	connect(tview, SIGNAL(requestedPage(int, bool)), this, SIGNAL(requestedPage(int, bool)));
	//connect(this, SIGNAL(orientationChanged(Orientation)), this, SLOT(onOrientationChanged(Orientation)));
}

ThumbnailsWindow::~ThumbnailsWindow()
{
}

void ThumbnailsWindow::customEvent(QEvent *e)
{
	if (e->type() == ThumbnailReady)
	{
		ThumbnailEvent *evt = dynamic_cast<ThumbnailEvent *>(e);
		tview->setPage(*evt->getThumbnail());
	}
	else
	{
		QDockWidget::customEvent(e);
	}
}

void ThumbnailsWindow::showEvent(QShowEvent *e)
{
	QDockWidget::showEvent(e);
	emit shown();
}

/*void ThumbnailsWindow::onOrientationChanged(Orientation o)
{
	tview->setArrangement(o == Qt::Horizontal ? QIconView::TopToBottom : QIconView::LeftToRight);
}*/

ThumbnailsView* ThumbnailsWindow::view() const
{
	return tview;
}

