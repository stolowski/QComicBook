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
#include "imlibimage.h"
#include <qpopupmenu.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qcursor.h>
#include <iostream>
#include <algorithm>

using namespace QComicBook;

const int ComicImageView::EXTRA_WHEEL_SPIN = 2;

ComicImageView::ComicImageView(QWidget *parent, Size size, Scaling scaling, const QColor &color): QScrollView(parent), isize(size), iscaling(scaling), iangle(0), xoff(0), yoff(0), lx(-1), wheelupcnt(0), wheeldowncnt(0), smallcursor(NULL)
{
	orgimage[0] = orgimage[1] = NULL;
        context_menu = new QPopupMenu(this);
        viewport()->setPaletteBackgroundColor(color);
        setFocusPolicy(QWidget::StrongFocus);
}

ComicImageView::~ComicImageView()
{
	if (orgimage[0])
		delete orgimage[0];
	if (orgimage[1])
		delete orgimage[1];
}

void ComicImageView::drawContents(QPainter *p, int clipx, int clipy, int clipw, int cliph)
{
	if (orgimage[0] == NULL)
		return;

	std::cout << "clipx=" << clipx << " clipy=" << clipy << " clipw=" << clipw << " cliph=" << cliph << " wasp=" << w_asp << " hasp=" << h_asp << std::endl;
        int px = clipx - xoff;
        int py = clipy - yoff;
              
        if (py < 0 ) //clear strip on the top
        {
                p->eraseRect(clipx, clipy, clipw, -py);
                py = 0;
        }
        if (px < 0) //clear strip on the left
        {
                p->eraseRect(clipx, yoff, -px, cliph);
                px = 0;
        }
       
        if (clipx + clipw < xoff || clipy + cliph < yoff)
                 return;
        //p->drawPixmap(std::max(xoff, clipx), std::max(yoff, clipy), *pixmap, px, py, clipw, cliph);
        //orgimage[0]->draw(p->device(), clipx, clipy, w_asp*clipw, h_asp*cliph, std::max(xoff-contentsX(), clipx-contentsX()), clipy-contentsY(), clipw, cliph);
        //orgimage[0]->draw(p->device(), clipx, clipy, w_asp*clipw, h_asp*cliph, std::max(xoff-contentsX(), clipx-contentsX()), std::max(yoff - contentsY(), clipy-contentsY()), clipw, cliph);
	
	double sx = w_asp * px;
	double sy = h_asp * py;

	double sw = w_asp*clipw;
	double sh = h_asp*cliph;
	
	double dx = std::max(xoff - contentsX(), clipx-contentsX());
	double dy = std::max(yoff - contentsY(), clipy-contentsY());

        orgimage[0]->draw(p->device(), (int)sx, (int)sy, sw, sh, dx, dy, clipw, cliph, iangle);

	//if ((sx + w_asp*clipw) > (orgimage[0]->width()*w_asp))
	{
		dx += orgimage[0]->width();
		if (orgimage[1])
		{
			std::cout << "img2\n";
			orgimage[1]->draw(p->device(), 0, (int)sy, w_asp*clipw, h_asp*cliph, dx, dy, clipw, cliph, iangle);
		}
	}


	/* OK
	 *
	double sx = w_asp * px;
	double sy = h_asp * py;
        orgimage[0]->draw(p->device(), (int)sx, (int)sy, w_asp*clipw, h_asp*cliph, std::max(xoff-contentsX(), clipx-contentsX()), std::max(yoff - contentsY(), clipy-contentsY()), clipw, cliph);
	*/
}

QPopupMenu *ComicImageView::contextMenu() const
{
        return context_menu;
}

bool ComicImageView::onTop()
{
        return contentsY() == 0;
}

bool ComicImageView::onBottom()
{
        return contentsY() + viewport()->height() >= contentsHeight();
}

void ComicImageView::contentsContextMenuEvent(QContextMenuEvent *e)
{
	if (orgimage[0])
                context_menu->popup(e->globalPos());
}

void ComicImageView::setImage(ImlibImage *img, bool preserveangle)
{
        if (!preserveangle)
                iangle = 0;

	if (orgimage[0])
		delete orgimage[0];

	if (orgimage[1])
	{
		delete orgimage[1];
		orgimage[1] = NULL;
	}

        orgimage[0] = img;

	if (iangle != 0)
		orgimage[0]->rotate(iangle);

        updateImageSize();
        ensureVisible(1, 1);

        repaintContents(0, 0 , viewport()->width(), viewport()->height());
}

void ComicImageView::setImage(ImlibImage *img1, ImlibImage *img2, bool preserveangle)
{
        if (!preserveangle)
                iangle = 0;

	if (orgimage[0])
		delete orgimage[0];

	if (orgimage[1])
	{
		delete orgimage[1];
		orgimage[1] = NULL;
	}

	orgimage[0] = img1;
	orgimage[1] = img2;

	if (iangle != 0)
	{
		orgimage[0]->rotate(iangle);
		orgimage[1]->rotate(iangle);
	}

        updateImageSize();
        ensureVisible(1, 1);

        repaintContents(0, 0 , viewport()->width(), viewport()->height());
}

void ComicImageView::resizeEvent(QResizeEvent *e)
{
        QScrollView::resizeEvent(e);
        updateImageSize();
}

void ComicImageView::contentsWheelEvent(QWheelEvent *e)
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
}

void ComicImageView::contentsMouseMoveEvent(QMouseEvent *e)
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
}

void ComicImageView::contentsMousePressEvent(QMouseEvent *e)
{
        if (!smallcursor)
                setCursor(Qt::PointingHandCursor);
}

void ComicImageView::contentsMouseReleaseEvent(QMouseEvent *e)
{
        lx = -1;
        if (!smallcursor)
                setCursor(Qt::ArrowCursor);
}


void ComicImageView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
	emit doubleClick();
}

void ComicImageView::updateImageSize()
{
	if (orgimage[0] == NULL)
		return;
        if (orgimage[0]->width() * orgimage[0]->height() == 0)
                return;

        Size size = isize;

	//
	// calculate iw, ih - the size of total image(s) area without scaling;
	// roatation angle of 2nd image is taken into account
	double iw = orgimage[0]->width();
	double ih = orgimage[0]->height();
	if (orgimage[1])
	{
		if (iangle & 1)
			ih += orgimage[1]->height();
		else
			iw += orgimage[1]->width();
	}
        
        if (size == BestFit)
        {
                if (iw > ih)
                {
                        if (iangle&1)
                                size = FitWidth;
                        else
                                size = FitHeight;
                }
                else
                {
                        if (iangle&1)
                                size = FitHeight;
                        else
                                size = FitWidth;
                }
        }

	int dw, dh;
                
	w_asp = h_asp = 1.0f;
        if (size == Original)
        {
		dw = iw;
		dh = ih;
	}
        else
        {       
		asp = (double)iw / ih;

		std::cout << "iw=" << iw << " ih=" << ih << " asp=" << asp << "\n";

		if (size == FitWidth)
		{
			w_asp = iw / viewport()->width();
			h_asp = w_asp * asp;
			h_asp = w_asp;
		}
		else if (size == FitHeight)
		{
			h_asp = ih / viewport()->height();
			w_asp = h_asp;
		}
		else if (size == WholePage)
		{
			w_asp = iw / viewport()->width();
			h_asp = ih / viewport()->height();
			if (w_asp > h_asp)
				h_asp = w_asp;
			else
				w_asp = h_asp;
		}

		dw = iw / w_asp;
		dh = ih / h_asp;
        }

	std::cout << "dw=" << dw << " dh=" << dh << "\n";
        int d;
        xoff = yoff = 0;

        //
        // calculate offsets for image centering
        if ((d = viewport()->width() - dw) > 0)
                xoff = d/2;
        if ((d = viewport()->height() - dh) > 0)
                yoff = d/2;
          
	std::cout << "xoff=" << xoff << " yoff=" << yoff << "\n";
	resizeContents(dw + xoff, dh + yoff);
                
        //
        // update scrolling speeds
        spdx = dw / 100;
        spdy = dh / 100;
}

void ComicImageView::setScaling(Scaling s)
{
        iscaling = s;
}

void ComicImageView::setRotation(Rotation r)
{
	if (r == QComicBook::Right)
		++iangle;
	else if (r == QComicBook::Left)
		--iangle;
	iangle &= 3;

	for (int i=0; i<2; i++)
		if (orgimage[i])
		{
			if (r == QComicBook::Right)
				orgimage[i]->rotate(1);
			else if (r == QComicBook::Left)
				orgimage[i]->rotate(3);
			else if (r == QComicBook::None && iangle != 0)
			{
				orgimage[i]->rotate(4-iangle);
				iangle = 0;
			}
		}

	updateImageSize();
	repaintContents(contentsX(), contentsY(), viewport()->width(), viewport()->height(), true);
}

void ComicImageView::setSize(Size s)
{
        isize = s;
        updateImageSize();
        repaintContents(contentsX(), contentsY(), viewport()->width(), viewport()->height(), true);
}

void ComicImageView::setSizeOriginal()
{
        setSize(Original);
}

void ComicImageView::setSizeFitWidth()
{
        setSize(FitWidth);
}

void ComicImageView::setSizeFitHeight()
{
        setSize(FitHeight);
}

void ComicImageView::setSizeWholePage()
{
        setSize(WholePage);
}

void ComicImageView::setSizeBestFit()
{
        setSize(BestFit);
}

void ComicImageView::scrollToTop()
{
        ensureVisible(1, 1);
}

void ComicImageView::scrollToBottom()
{
        ensureVisible(1, contentsHeight());
}

void ComicImageView::scrollRight()
{
        scrollBy(spdx, 0);
}

void ComicImageView::scrollLeft()
{
        scrollBy(-spdx, 0);
}

void ComicImageView::scrollRightFast()
{
        scrollBy(3*spdx, 0);
}

void ComicImageView::scrollLeftFast()
{
        scrollBy(-3*spdx, 0);
}

void ComicImageView::scrollUp()
{
        if (onTop())
        {
                wheelupcnt = wheeldowncnt = 0;
                emit topReached();
        }
        else
                scrollBy(0, -spdy);
}

void ComicImageView::scrollDown()
{
        if (onBottom())
        {
                wheelupcnt = wheeldowncnt = 0;
                emit bottomReached();
        }
        else
                scrollBy(0, spdy);
}

void ComicImageView::scrollUpFast()
{
        if (onTop())
                emit topReached();
        else
                scrollBy(0, -3*spdy);
}

void ComicImageView::scrollDownFast()
{       
        if (onBottom())
                emit bottomReached();
        else
                scrollBy(0, 3*spdy);
}

void ComicImageView::rotateRight()
{
        setRotation(QComicBook::Right);
}

void ComicImageView::rotateLeft()
{
        setRotation(QComicBook::Left);
}

void ComicImageView::resetRotation()
{
        setRotation(None);
}

void ComicImageView::jumpUp()
{
        if (onTop())
                emit topReached();
        else
                scrollBy(0, -viewport()->height());
}

void ComicImageView::jumpDown()
{
        if (onBottom())
                emit bottomReached();
        else
                scrollBy(0, viewport()->height());
}

void ComicImageView::enableScrollbars(bool f)
{
        ScrollBarMode s = f ? Auto : AlwaysOff;
        setVScrollBarMode(s);
        setHScrollBarMode(s);
}

void ComicImageView::setBackground(const QColor &color)
{
        viewport()->setPaletteBackgroundColor(color);
}

void ComicImageView::setSmallCursor(bool f)
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
}

void ComicImageView::clear()
{
	if (orgimage[0])
		orgimage[0]->reset();
	if (orgimage[1])
		orgimage[1]->reset();
        resizeContents(0, 0);
}

Size ComicImageView::getSize() const
{
        return isize;
}

int ComicImageView::imageWidth() const
{
	if (orgimage[0])
		return orgimage[0]->width();
	return 0;
}

