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
#include <QImage>
#include <QPixmap>
#include <QMenu>
#include <QScrollBar>
#include <QContextMenuEvent>
#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <qbitmap.h>
#include <qcursor.h>
#include <algorithm>

using namespace QComicBook;

const int ComicImageView::EXTRA_WHEEL_SPIN = 3;
const float ComicImageView::JUMP_FACTOR = 0.85f;

ComicImageView::ComicImageView(QWidget *parent, Size size, const QColor &color): QScrollArea(parent),
	isize(size), iangle(0), xoff(0), yoff(0), imgs(0), totalWidth(0), totalHeight(0),
	lx(-1),	wheelupcnt(0), wheeldowncnt(0), smallcursor(NULL)
{
        context_menu = new QMenu(this);
        //setFocusPolicy(QWidget::StrongFocus);
	imgLabel = new QLabel();
	imgLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imgLabel->setScaledContents(true);
	setWidget(imgLabel);

	setBackground(color);
	//setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

ComicImageView::~ComicImageView()
{
}

void ComicImageView::scrollByDelta(int dx, int dy)
{
	QScrollBar *vbar = verticalScrollBar();
	QScrollBar *hbar = horizontalScrollBar();

	vbar->setValue(vbar->value() + dy);
	hbar->setValue(hbar->value() + dx);
}

QMenu *ComicImageView::contextMenu() const
{
        return context_menu;
}

bool ComicImageView::onTop()
{
        return verticalScrollBar()->value() == verticalScrollBar()->minimum();
}

bool ComicImageView::onBottom()
{
        return verticalScrollBar()->value() == verticalScrollBar()->maximum();
}

void ComicImageView::contextMenuEvent(QContextMenuEvent *e)
{
	if (imgs > 0)
		context_menu->popup(e->globalPos());
}

void ComicImageView::setImage(const QImage &img1, bool preserveangle)
{
        if (!preserveangle)
                iangle = 0;

	imgs = 1;
	orgimg[0] = img1;

        redrawImages();
}

void ComicImageView::setImage(const QImage &img1, const QImage &img2, bool preserveangle)
{
        if (!preserveangle)
                iangle = 0;

	imgs = 2;
	orgimg[0] = img1;
	orgimg[1] = img2;

	redrawImages();
}

void ComicImageView::resizeEvent(QResizeEvent *e)
{
        QScrollArea::resizeEvent(e);
        updateImageSize();
}

void ComicImageView::wheelEvent(QWheelEvent *e)
{
        e->accept();
        if (e->delta() > 0) //scrolling up
        {
                if (imgLabel->height() <= height() || (onTop() && ++wheelupcnt > EXTRA_WHEEL_SPIN))
                {
                        wheelupcnt = 0;
                        emit topReached();
                }
                else
                {
                        scrollByDelta(0, -3*spdy);
                        wheeldowncnt = 0; //reset opposite direction counter
                }
        }
        else //scrolling down
        {
                if (imgLabel->height() <= height() || (onBottom() && ++wheeldowncnt > EXTRA_WHEEL_SPIN))
                {
                        wheeldowncnt = 0;
                        emit bottomReached();
                }
                else
                {
                        scrollByDelta(0, 3*spdy);
                        wheelupcnt = 0; //reset opposite direction counter
                }
        }
}

void ComicImageView::mouseMoveEvent(QMouseEvent *e)
{
        if (lx >= 0)
        {
                const int dx = lx - e->x();
                const int dy = ly - e->y();

		QScrollBar *vbar = verticalScrollBar();
		QScrollBar *hbar = horizontalScrollBar();

		vbar->setValue(vbar->value() + dy);
		hbar->setValue(hbar->value() + dx);
        }
        lx = e->x();
        ly = e->y();
}

void ComicImageView::mousePressEvent(QMouseEvent *e)
{
        if (!smallcursor)
                setCursor(Qt::PointingHandCursor);
}

void ComicImageView::mouseReleaseEvent(QMouseEvent *e)
{
        lx = -1;
	ly = -1;
        if (!smallcursor)
                setCursor(Qt::ArrowCursor);
}

void ComicImageView::mouseDoubleClickEvent(QMouseEvent *e)
{
	e->accept();
	emit doubleClick();
}

void ComicImageView::updateImageSize()
{
	if (totalWidth * totalHeight  * imgs == 0)
		return;

	Size size = isize;

	const double hRatio = static_cast<double>(height()) / totalHeight;
	const double wRatio = static_cast<double>(width()) / totalWidth;

	int w, h;

        if (size == BestFit)
        {
		if (totalWidth > totalHeight)
			size = FitWidth;
		else
			size = FitHeight;
        }
       	if (size == Original)
        {
		w = totalWidth;
		h = totalHeight;
        }
	else if (size == FitWidth)
	{
		w = width();
		h = static_cast<int>(static_cast<double>(totalHeight) * wRatio);
	}
	else if (size == FitHeight)
	{
		w = static_cast<int>(static_cast<double>(totalWidth) * hRatio);
		h = height();
	}
	else if (size == WholePage)
	{
		const double ratio = std::min(wRatio, hRatio);
		w = static_cast<int>(static_cast<double>(ratio) * totalWidth);
		h = static_cast<int>(static_cast<double>(ratio) * totalHeight);
	}

	xoff = (width() - w) /2 ;
	yoff = (height() - h) /2;

	if (xoff < 0)
		xoff = 0;
	if (yoff < 0)
		yoff = 0;
	imgLabel->setContentsMargins(xoff, yoff, 0, 0);
	imgLabel->setFixedSize(w + xoff, h + yoff);
	
	//
        // update scrolling speeds
        spdx = w / 100;
        spdy = h / 100;
}

void ComicImageView::redrawImages()
{
	if (imgs < 1)
		return;

	if (imgs  == 1)
	{
		if (iangle == 0 || iangle == 2)
		{
			totalWidth = orgimg[0].width();
			totalHeight = orgimg[0].height();
		}
		else
		{
			totalWidth = orgimg[0].height();
			totalHeight = orgimg[0].width();
		}
	}
	else
	{
		if (iangle == 0 || iangle == 2)
		{
			totalWidth = orgimg[0].width() + orgimg[1].width();
			totalHeight = std::max(orgimg[0].height(), orgimg[1].height());
		}
		else
		{
			totalWidth = std::max(orgimg[0].height(), orgimg[1].height());
			totalHeight = orgimg[0].width() + orgimg[1].width();
		}
	}
	
	QPixmap pixmap(totalWidth, totalHeight);
	QPainter p(&pixmap);
	if (iangle > 0)
	{
		rmtx.reset();
		if (iangle == 1)
			rmtx.translate(totalWidth, 0);
		else if (iangle == 3)
			rmtx.translate(0, totalHeight);
		else
			rmtx.translate(totalWidth, totalHeight);
		rmtx.rotate(static_cast<double>(iangle) * 90.0f);
		p.setWorldMatrix(rmtx);
		p.setWorldMatrixEnabled(true);
	}
	if (imgs  == 1)
	{
		p.drawImage(0, 0, orgimg[0], 0, 0);
	}
	else
	{
		p.drawImage(0, 0, orgimg[0], 0, 0);
		p.drawImage(orgimg[0].width(), 0, orgimg[1], 0, 0);
	}
	p.end();
	imgLabel->setPixmap(pixmap);
	imgLabel->adjustSize();

	updateImageSize();
}

void ComicImageView::setRotation(Rotation r)
{
	if (r == QComicBook::None && iangle == 0)
		return;

        if (r == QComicBook::Right)
                ++iangle;
        else if (r == QComicBook::Left)
                --iangle;
        else
                iangle = 0; //None
        iangle &= 3;            

	redrawImages();
}

void ComicImageView::setSize(Size s)
{
        isize = s;
        updateImageSize();
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
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
}

void ComicImageView::scrollToBottom()
{
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void ComicImageView::scrollRight()
{
        scrollByDelta(spdx, 0);
}

void ComicImageView::scrollLeft()
{
        scrollByDelta(-spdx, 0);
}

void ComicImageView::scrollRightFast()
{
        scrollByDelta(3*spdx, 0);
}

void ComicImageView::scrollLeftFast()
{
        scrollByDelta(-3*spdx, 0);
}

void ComicImageView::scrollUp()
{
        if (onTop())
        {
                wheelupcnt = wheeldowncnt = 0;
                emit topReached();
        }
        else
		scrollByDelta(0, -spdy);
}

void ComicImageView::scrollDown()
{
        if (onBottom())
        {
                wheelupcnt = wheeldowncnt = 0;
                emit bottomReached();
        }
        else
		scrollByDelta(0, spdy);
}

void ComicImageView::scrollUpFast()
{
        if (onTop())
                emit topReached();
        else
		scrollByDelta(0, -3*spdy);
}

void ComicImageView::scrollDownFast()
{       
        if (onBottom())
                emit bottomReached();
        else
		scrollByDelta(0, 3*spdy);
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
		scrollByDelta(0, -static_cast<int>(JUMP_FACTOR * viewport()->height()));
}

void ComicImageView::jumpDown()
{
        if (onBottom())
                emit bottomReached();
        else 
		scrollByDelta(0, static_cast<int>(JUMP_FACTOR * viewport()->height()));
}

void ComicImageView::enableScrollbars(bool f)
{
        const Qt::ScrollBarPolicy s = f ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff;
        setVerticalScrollBarPolicy(s);
        setHorizontalScrollBarPolicy(s);
}

void ComicImageView::setBackground(const QColor &color)
{
	QPalette palette;
	palette.setColor(backgroundRole(), color);
	setPalette(palette);
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
                const QBitmap bmp = QBitmap::fromData(QSize(32, 32), bmp_bits, QImage::Format_Mono);
                const QBitmap msk = QBitmap::fromData(QSize(32, 32), msk_bits, QImage::Format_Mono);
                smallcursor = new QCursor(bmp, msk, 0, 0);
                setCursor(*smallcursor);
        }
        else
        {
                if (smallcursor)
                        delete smallcursor;
                smallcursor = NULL;
		unsetCursor();
        }
}

void ComicImageView::clear()
{
	imgs = 0;
	imgLabel->setFixedSize(0, 0);
}

Size ComicImageView::getSize() const
{
        return isize;
}
			
int ComicImageView::visiblePages() const
{
	return imgs;
}

const QPixmap ComicImageView::image() const
{
	if (imgs > 0)
		return QPixmap(*imgLabel->pixmap());
	return QPixmap(); //fallback
}

int ComicImageView::viewWidth() const
{
	return (imgs > 0) ? imgLabel->width() : 0;
}


