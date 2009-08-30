/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
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
#include <QtGlobal>
#include <algorithm>

using namespace QComicBook;

const int ComicImageView::EXTRA_WHEEL_SPIN = 3;
const float ComicImageView::JUMP_FACTOR = 0.85f;

ComicImageView::ComicImageView(QWidget *parent, int physicalPages, bool twoPagesMode, Size size, const QColor &color)
  : QScrollArea(parent)
  , props(size, 0, false, true)
  , m_physicalPages(physicalPages)
  , m_twoPagesMode(twoPagesMode)
  , m_contMode(false)
  , lx(-1), wheelupcnt(0), wheeldowncnt(0), smallcursor(NULL)
{
    context_menu = new QMenu(this);
    //setFocusPolicy(QWidget::StrongFocus);
 
    QWidget *w = new QWidget(this);
    w->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    m_layout = new QVBoxLayout(w);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setAlignment(Qt::AlignCenter);
    recreatePageWidgets();
    setWidget(w);
    setWidgetResizable(true);
    
    setBackground(color);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(&props, SIGNAL(changed()), this, SLOT(propsChanged()));
}

ComicImageView::~ComicImageView()
{
}

void ComicImageView::setNumOfPages(int n)
{
    m_physicalPages = n;
    recreatePageWidgets();
}

void ComicImageView::propsChanged()
{
    foreach (PageWidget *p, imgLabel)
    {
        p->redrawImages();
    }
    disposeOrRequestPages();
    update();
}

void ComicImageView::recreatePageWidgets()
{
    foreach (PageWidget *p, imgLabel)
    {
        delete p;
    }
    imgLabel.clear();

    if (m_twoPagesMode)
    {
        for (int i=0; i<m_physicalPages/2; i++) //TODO
        {
            PageWidget *p = new PageWidget(this, i);
            imgLabel.append(p);
            m_layout->addWidget(p);
        }
    }
    else
    {
        for (int i=0; i<m_physicalPages; i++)
        {
            PageWidget *p = new PageWidget(this, i);
            p->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            imgLabel.append(p);
            m_layout->addWidget(p);
        }
    }
}

PageWidget* ComicImageView::findPageWidget(int pageNum) const
{
    if (m_twoPagesMode)
    {
        pageNum /= 2;
    }
    if (pageNum < imgLabel.size())
    {
        return imgLabel[pageNum];
    }
    return NULL;
}

void ComicImageView::disposeUnneededPageWidgets()
{
    int i;
    for (i=0; i<imgLabel.size(); i++)
    {
        PageWidget *w = imgLabel[i];
        if (!w->isInView() && !w->estimatedSize() && !w->isDisposed())
        {
            break;
        }
    }
    while (i<imgLabel.size())
    {
        PageWidget *w = imgLabel[i];
        if (!w->isInView() && !w->estimatedSize() && !w->isDisposed())
        {
            w->dispose();
        }
        else
        {
            break;
        }
        ++i;
    }
}

void ComicImageView::disposeOrRequestPages()
{
    for (int i=0; i<imgLabel.size(); i++)
    {
        PageWidget *w = imgLabel[i];
        if (w->isInView())
        {
            if (w->estimatedSize() || w->isDisposed())
            {
                if (m_twoPagesMode)
                {
                    emit requestTwoPages(w->pageNumber());
                }
                else
                {
                    emit requestPage(w->pageNumber());
                }
            }
        }
        else
        {
            if (!w->estimatedSize() && !w->isDisposed())
            {
                // dispose page only if none of its neighbours are in view
                if (! ((i>1 && imgLabel[i-1]->isInView()) || (i<imgLabel.size()-1 && imgLabel[i+1]->isInView())) )
                {
                    w->dispose();
                }
            }
        }
    }
}

QList<PageWidget *> ComicImageView::findPageWidgetsInView() const
{
    QList<PageWidget *> wlist;
    int i;
    for (i=0; i<imgLabel.size(); i++)
    {
        if (imgLabel[i]->isInView())
        {
            break;
        }
    }
    while (i < imgLabel.size() && imgLabel[i]->isInView()) 
    {
        wlist.append(imgLabel[i]);
        ++i;
    }
    return wlist;
}

void ComicImageView::recalculatePageSizes()
{
    if (imgLabel.size() > 0)
    {
        int avgw = 0;
        int avgh = 0;
        int n = 0;
        foreach (PageWidget *p, imgLabel)
        {
            if (!p->estimatedSize())
            {
                const QSize s(p->size());
                avgw += s.width();
                avgh += s.height();
                ++n;
            }
        }
        if (n > 0)
        {
            avgw /= n;
            avgh /= n;

            foreach (PageWidget *p, imgLabel)
            {
                if (p->estimatedSize())
                {
                    p->setEstimatedSize(avgw, avgh);
                }
            }
        }
    }
}

void ComicImageView::scrollByDelta(int dx, int dy)
{
	QScrollBar *vbar = verticalScrollBar();
	QScrollBar *hbar = horizontalScrollBar();

	vbar->setValue(vbar->value() + dy);
	hbar->setValue(hbar->value() + dx);
}

void ComicImageView::scrollContentsBy(int dx, int dy)
{
    QScrollArea::scrollContentsBy(dx, dy);
    disposeOrRequestPages();
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
    Q_ASSERT(m_physicalPages > 0);
    //if (!preserveangle)
    //          iangle = 0;
    PageWidget *w = findPageWidget(img1.getNumber());
    Q_ASSERT(w != NULL);
    w->setImage(img1);
    recalculatePageSizes();
}

void ComicImageView::setImage(const Page &img1, const Page &img2, bool preserveangle)
{
    Q_ASSERT(m_physicalPages > 0);
    //  if (!preserveangle)
    //          iangle = 0;
    PageWidget *w = findPageWidget(img1.getNumber());
    Q_ASSERT(w != NULL);
    w->setImage(img1, img2);
    recalculatePageSizes();
}

void ComicImageView::resizeEvent(QResizeEvent *e)
{
    QScrollArea::resizeEvent(e);
    foreach (PageWidget *w, imgLabel)
    {
        w->redrawImages();
    }
    disposeOrRequestPages();
}

void ComicImageView::wheelEvent(QWheelEvent *e)
{
//TODO
    /*e->accept();
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
                }*/
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

void ComicImageView::setTwoPagesMode(bool f)
{
    if (f != m_twoPagesMode)
    {
        m_twoPagesMode = f;
        recreatePageWidgets();
    }
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
    m_physicalPages = 0;
    recreatePageWidgets();
}

Size ComicImageView::getSize() const
{
    return props.size();
}
			
int ComicImageView::visiblePages() const
{
    return m_twoPagesMode ? 2 : 1;
    //return imgLabel->numOfPages();
}

const QPixmap ComicImageView::image() const
{
//TODO
//    if (imgLabel->numOfPages())
//        return QPixmap(*imgLabel->pixmap());
    return QPixmap(); //fallback
}

int ComicImageView::viewWidth() const
{
    return width(); //TODO?
//    return (imgLabel->numOfPages()) ? imgLabel->width() : 0;
}

ViewProperties& ComicImageView::properties()
{
    return props;
}
