/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ComicImageView.h"
#include "Utility.h"
#include "PageWidget.h"
#include <QImage>
#include <QPixmap>
#include <QMenu>
#include <QScrollBar>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QBitmap>
#include <QCursor>
#include <algorithm>

using namespace QComicBook;

const int ComicImageView::EXTRA_WHEEL_SPIN = 3;
const float ComicImageView::JUMP_FACTOR = 0.85f;

ComicImageView::ComicImageView(QWidget *parent, Size size, const QColor &color)
  : QScrollArea(parent)
  , props(size, 0, false)
  , lx(-1), wheelupcnt(0), wheeldowncnt(0), smallcursor(NULL)
{
    context_menu = new QMenu(this);
    //setFocusPolicy(QWidget::StrongFocus);
    imgLabel = new PageWidget(this);
    imgLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setWidget(imgLabel);
    
    setBackground(color);
    //setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(&props, SIGNAL(changed()), imgLabel, SLOT(propsChanged()));
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
    //if (imgs > 0)
    context_menu->popup(e->globalPos());
}

void ComicImageView::setImage(const Page &img1, bool preserveangle)
{
    //if (!preserveangle)
    //          iangle = 0;

        imgLabel->setImage(img1);
}

void ComicImageView::setImage(const Page &img1, const Page &img2, bool preserveangle)
{
    //  if (!preserveangle)
    //          iangle = 0;

        imgLabel->setImage(img1), img2;
}

void ComicImageView::resizeEvent(QResizeEvent *e)
{
        QScrollArea::resizeEvent(e);
        imgLabel->redrawImages();
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

void ComicImageView::setRotation(Rotation r)
{
    props.setAngle(r);
}

void ComicImageView::setSize(Size s)
{
    props.setSize(s);
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
    props.setBackground(color);
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

void ComicImageView::showPageNumbers(bool f)
{
    props.setPageNumbers(f);
}

void ComicImageView::clear()
{
    imgLabel->dispose();
}

Size ComicImageView::getSize() const
{
    return props.size();
}
			
int ComicImageView::visiblePages() const
{
    return imgLabel->numOfPages();
}

const QPixmap ComicImageView::image() const
{
    if (imgLabel->numOfPages())
        return QPixmap(*imgLabel->pixmap());
    return QPixmap(); //fallback
}

int ComicImageView::viewWidth() const
{
    return (imgLabel->numOfPages()) ? imgLabel->width() : 0;
}

ViewProperties& ComicImageView::properties()
{
    return props;
}
