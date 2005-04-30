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

#include "imgview.h"
#include "miscutil.h"
#include <qimage.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qcursor.h>

const int ComicImageView::EXTRA_WHEEL_SPIN = 2;

ComicImageView::ComicImageView(QWidget *parent, ComicImageView::Size size, ComicImageView::Scaling scaling, const QColor &color): QScrollView(parent), isize(size), iscaling(scaling), xoff(0), yoff(0), lx(-1), wheelupcnt(0), wheeldowncnt(0), smallcursor(NULL)/*{{{*/
{
	orgimage = new QImage();
	pixmap = new QPixmap();
	context_menu = new QPopupMenu(this);
	viewport()->setPaletteBackgroundColor(color);
	setFocusPolicy(QWidget::StrongFocus);
}/*}}}*/

ComicImageView::~ComicImageView()/*{{{*/
{
	delete pixmap;
	delete orgimage;
}/*}}}*/

void ComicImageView::drawContents(QPainter *p, int clipx, int clipy, int clipw, int cliph)/*{{{*/
{
	const int px = clipx - xoff;
	const int py = clipy - yoff;
	
	if (py < 0 ) //clear strip on the top
		p->eraseRect(clipx, clipy, clipw, -py);//, QColor(0xca, 0xca, 0xca));

	if (px < 0) //clear strip on the left
		p->eraseRect(clipx, yoff /*clipy*/, -px, cliph); // QColor(0xcf, 0xcf, 0xcf));
	/*if (px + clipw < pixmap->width())
		p->fillRect(px + clipw, clipy, -px, cliph, QColor(0x40, 0x40, 0x40));*/
	
	p->drawPixmap(clipx, clipy, *pixmap, px, py, clipw, cliph);
}/*}}}*/

QPopupMenu *ComicImageView::contextMenu() const/*{{{*/
{
	return context_menu;
}/*}}}*/

bool ComicImageView::onTop()/*{{{*/
{
	return contentsY() == 0;
}/*}}}*/

bool ComicImageView::onBottom()/*{{{*/
{
	return contentsY() + viewport()->height() >= contentsHeight();
}/*}}}*/

void ComicImageView::contentsContextMenuEvent(QContextMenuEvent *e)/*{{{*/
{
	if (!pixmap->isNull())
		context_menu->popup(e->globalPos());
}/*}}}*/

void ComicImageView::setImage(const QImage &img)/*{{{*/
{
	*orgimage = img;

	updateImageSize();
	ensureVisible(1, 1);

	repaintContents(0, 0 , viewport()->width(), viewport()->height());
}/*}}}*/

void ComicImageView::setImage(const QImage &img1, const QImage &img2)/*{{{*/
{
	//
	// we need to recreate orgimage as it may refer to original image data in cache;
	// we dont want to work on shallow copy; it is probably also better than detach()
	delete orgimage;
	orgimage = new QImage(img1.width() + img2.width(), maxOfTwo(img1.height(), img2.height()), 32);

	bitBlt(orgimage, 0, 0, &img1, 0, 0, -1, -1, 0);
	bitBlt(orgimage, img1.width(), 0, &img2, 0, 0, -1, -1, 0);

	updateImageSize();
	ensureVisible(1, 1);

	repaintContents(0, 0 , viewport()->width(), viewport()->height());
}/*}}}*/

void ComicImageView::resizeEvent(QResizeEvent *e)/*{{{*/
{
	QScrollView::resizeEvent(e);
	updateImageSize();
}/*}}}*/

void ComicImageView::contentsWheelEvent(QWheelEvent *e)/*{{{*/
{
	e->accept();
	if (e->delta() > 0) //scrolling up
	{
		if (contentsHeight()<=viewport()->height() || (onTop() && ++wheelupcnt > EXTRA_WHEEL_SPIN))
		{
			emit topReached();
			wheelupcnt = 0;
		}
		else
		{
			scrollBy(0, -3*spdy);
			wheeldowncnt = 0; //reset opposite direction counter
		}
	}
	else //scrolling down
	{
		if (contentsHeight()<=viewport()->height() || (onBottom() && ++wheeldowncnt > EXTRA_WHEEL_SPIN))
		{
			emit bottomReached();
			wheeldowncnt = 0;
		}
		else
		{
			scrollBy(0, 3*spdy);
			wheelupcnt = 0; //reset opposite direction counter
		}
	}
}/*}}}*/

void ComicImageView::contentsMouseMoveEvent(QMouseEvent *e)/*{{{*/
{
	if (lx >= 0)
	{
		const int dx = lx - e->x();
		const int dy = ly - e->y();
		scrollBy(dx, dy);
		lx = e->x() + dx; //need to add delta because e->x() is the old position
		ly = e->y() + dy;
	}
	else 
	{
		lx = e->x();
		ly = e->y();
	}
}/*}}}*/

void ComicImageView::contentsMousePressEvent(QMouseEvent *e)/*{{{*/
{
	if (!smallcursor)
		setCursor(Qt::PointingHandCursor);
}/*}}}*/

void ComicImageView::contentsMouseReleaseEvent(QMouseEvent *e)/*{{{*/
{
	lx = -1;
	if (!smallcursor)
		setCursor(Qt::ArrowCursor);
}/*}}}*/

void ComicImageView::updateImageSize()/*{{{*/
{
	if (orgimage->width() * orgimage->height() == 0)
		return;

	Size size = isize;
	
	if (size == BestFit)
	{
		if (orgimage->width() > orgimage->height())
				size = FitHeight;
		else
			size = FitWidth;
	}
		
	if (size == Original)
		pixmap->convertFromImage(*orgimage);
	else
	{	
		QImage img;
		int w = viewport()->width();
		int h = viewport()->height();

		if (size == FitWidth)
			h = 65535;
		else if (size == FitHeight)
			w = 65535;

		if (iscaling == Smooth)
			img = orgimage->smoothScale(w, h, QImage::ScaleMin);
		else if (iscaling == Fast)
			img = orgimage->scale(w, h, QImage::ScaleMin);
	
		pixmap->convertFromImage(img);
	}

	int d;
	xoff = yoff = 0;

	//
	// calculate offsets for image centering
	if ((d = viewport()->width() - pixmap->width()) > 0)
		xoff = d/2;
	if ((d = viewport()->height() - pixmap->height()) > 0)
		yoff = d/2;
			
	resizeContents(pixmap->width() + xoff, pixmap->height() + yoff);
		
	//
	// update scrolling speeds
	spdx = pixmap->width() / 100;
	spdy = pixmap->height() / 100;
}/*}}}*/

void ComicImageView::setScaling(ComicImageView::Scaling s)/*{{{*/
{
	iscaling = s;
}/*}}}*/

void ComicImageView::setSize(Size s)/*{{{*/
{
	isize = s;
	updateImageSize();
	updateContents(0, 0, viewport()->width(), viewport()->height());
}/*}}}*/

void ComicImageView::setSizeOriginal()/*{{{*/
{
	setSize(Original);
}/*}}}*/

void ComicImageView::setSizeFitWidth()/*{{{*/
{
	setSize(FitWidth);
}/*}}}*/

void ComicImageView::setSizeFitHeight()/*{{{*/
{
	setSize(FitHeight);
}/*}}}*/

void ComicImageView::setSizeWholePage()/*{{{*/
{
	setSize(WholePage);
}/*}}}*/

void ComicImageView::setSizeBestFit()/*{{{*/
{
	setSize(BestFit);
}/*}}}*/

void ComicImageView::scrollToTop()/*{{{*/
{
	ensureVisible(1, 1);
}/*}}}*/

void ComicImageView::scrollToBottom()/*{{{*/
{
	ensureVisible(1, contentsHeight());
}/*}}}*/

void ComicImageView::scrollRight()/*{{{*/
{
	scrollBy(spdx, 0);
}/*}}}*/

void ComicImageView::scrollLeft()/*{{{*/
{
	scrollBy(-spdx, 0);
}/*}}}*/

void ComicImageView::scrollRightFast()/*{{{*/
{
	scrollBy(3*spdx, 0);
}/*}}}*/

void ComicImageView::scrollLeftFast()/*{{{*/
{
	scrollBy(-3*spdx, 0);
}/*}}}*/

void ComicImageView::scrollUp()/*{{{*/
{
	if (onTop())
	{
		wheelupcnt = wheeldowncnt = 0;
		emit topReached();
	}
	else
		scrollBy(0, -spdy);
}/*}}}*/

void ComicImageView::scrollDown()/*{{{*/
{
	if (onBottom())
	{
		wheelupcnt = wheeldowncnt = 0;
		emit bottomReached();
	}
	else
		scrollBy(0, spdy);
}/*}}}*/

void ComicImageView::scrollUpFast()/*{{{*/
{
	if (onTop())
		emit topReached();
	else
		scrollBy(0, -3*spdy);
}/*}}}*/

void ComicImageView::scrollDownFast()/*{{{*/
{	
	if (onBottom())
		emit bottomReached();
	else
		scrollBy(0, 3*spdy);
}/*}}}*/

void ComicImageView::jumpUp()/*{{{*/
{
	if (onTop())
		emit topReached();
	else
		scrollBy(0, -viewport()->height());
}/*}}}*/

void ComicImageView::jumpDown()/*{{{*/
{
	if (onBottom())
		emit bottomReached();
	else
		scrollBy(0, viewport()->height());
}/*}}}*/

void ComicImageView::enableScrollbars(bool f)/*{{{*/
{
	ScrollBarMode s = f ? Auto : AlwaysOff;
	setVScrollBarMode(s);
	setHScrollBarMode(s);
}/*}}}*/

void ComicImageView::setBackground(const QColor &color)/*{{{*/
{
	viewport()->setPaletteBackgroundColor(color);
}/*}}}*/

void ComicImageView::setSmallCursor(bool f)/*{{{*/
{
	if (f)
	{
		static unsigned char bmp_bits[4*32];
		static unsigned char msk_bits[4*32];

		if (smallcursor)
			return;

		for (int i=0; i<4*32; i++)
			bmp_bits[i] = msk_bits[i] = 0;
		bmp_bits[0] = msk_bits[0] = 0xe0;
		bmp_bits[4] = 0xa0;
		msk_bits[4] = 0xe0;
		bmp_bits[8] = msk_bits[8] = 0xe0;
		const QBitmap bmp(32, 32, bmp_bits, false);
		const QBitmap msk(32, 32, msk_bits, false);
		smallcursor = new QCursor(bmp, msk, 0, 0);
		setCursor(*smallcursor);
	}
	else
	{
		if (smallcursor)
			delete smallcursor;
		smallcursor = NULL;
		setCursor(Qt::ArrowCursor);
	}
}/*}}}*/

void ComicImageView::clear()/*{{{*/
{
	orgimage->reset();
	pixmap->resize(0, 0);
	resizeContents(0, 0);
}/*}}}*/

ComicImageView::Size ComicImageView::getSize() const/*{{{*/
{
	return isize;
}/*}}}*/

