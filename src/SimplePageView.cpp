#include "SimplePageView.h"

#include "ContinuousPageView.h"
#include "Utility.h"
#include "PageWidget.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QtGlobal>
#include <QResizeEvent>
#include <QScrollBar>
#include <QDebug>
#include <algorithm>

using namespace QComicBook;

const int SimplePageView::EXTRA_WHEEL_SPIN = 3;
const float SimplePageView::JUMP_FACTOR = 0.85f;

SimplePageView::SimplePageView(QWidget *parent, int physicalPages, bool twoPagesMode, Size size, const QColor &color)
    : PageViewBase(parent, twoPagesMode, size, color)
    , wheelupcnt(0), wheeldowncnt(0)
    , m_physicalPages(physicalPages)
{
    //setFocusPolicy(QWidget::StrongFocus);
 
//    w->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
//TODO
    setWidget(imgLabel);
//    setWidgetResizable(true);
    
    setBackground(color);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

SimplePageView::~SimplePageView()
{
}

void SimplePageView::setNumOfPages(int n)
{
    m_physicalPages = n;
//TODO
}

void SimplePageView::propsChanged()
{
}

void SimplePageView::scrollContentsBy(int dx, int dy)
{
    PageViewBase::scrollContentsBy(dx, dy);
}

void SimplePageView::setImage(const Page &img1)
{
    Q_ASSERT(m_physicalPages > 0);
    //if (!preserveangle)
    //          iangle = 0;
    imgLabel->setImage(img1);
}

void SimplePageView::setImage(const Page &img1, const Page &img2)
{
    Q_ASSERT(m_physicalPages > 0);
    //  if (!preserveangle)
    //          iangle = 0;
    imgLabel->setImage(img1, img2);
}

void SimplePageView::resizeEvent(QResizeEvent *e)
{
    QScrollArea::resizeEvent(e);
    imgLabel->redrawImages();
    e->accept();
}

void SimplePageView::wheelEvent(QWheelEvent *e)
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


void SimplePageView::setTwoPagesMode(bool f)
{
    /*if (f != m_twoPagesMode)
    {
        m_twoPagesMode = f;
        recreatePageWidgets();
        }*/
    props.setTwoPagesMode(f);
}

void SimplePageView::scrollRight()
{
        scrollByDelta(spdx, 0);
}

void SimplePageView::scrollLeft()
{
        scrollByDelta(-spdx, 0);
}

void SimplePageView::scrollRightFast()
{
        scrollByDelta(3*spdx, 0);
}

void SimplePageView::scrollLeftFast()
{
        scrollByDelta(-3*spdx, 0);
}

void SimplePageView::scrollUp()
{
        if (onTop())
        {
                wheelupcnt = wheeldowncnt = 0;
                emit topReached();
        }
        else
		scrollByDelta(0, -spdy);
}

void SimplePageView::scrollDown()
{
        if (onBottom())
        {
                wheelupcnt = wheeldowncnt = 0;
                emit bottomReached();
        }
        else
		scrollByDelta(0, spdy);
}

void SimplePageView::scrollUpFast()
{
        if (onTop())
                emit topReached();
        else
		scrollByDelta(0, -3*spdy);
}

void SimplePageView::scrollDownFast()
{       
        if (onBottom())
                emit bottomReached();
        else
		scrollByDelta(0, 3*spdy);
}

void SimplePageView::rotateRight()
{
        setRotation(QComicBook::Right);
}

void SimplePageView::rotateLeft()
{
        setRotation(QComicBook::Left);
}

void SimplePageView::resetRotation()
{
        setRotation(None);
}

void SimplePageView::jumpUp()
{
        if (onTop())
                emit topReached();
        else 
		scrollByDelta(0, -static_cast<int>(JUMP_FACTOR * viewport()->height()));
}

void SimplePageView::jumpDown()
{
        if (onBottom())
                emit bottomReached();
        else 
		scrollByDelta(0, static_cast<int>(JUMP_FACTOR * viewport()->height()));
}

void SimplePageView::scrollToTop()
{
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
}

void SimplePageView::scrollToBottom()
{
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void SimplePageView::clear()
{
    m_physicalPages = 0;
    delRequests();
}
			
int SimplePageView::visiblePages() const
{
    return props.twoPagesMode() ? 2 : 1;
    //return imgLabel->numOfPages();
}

const QPixmap SimplePageView::image() const
{
//TODO
//    if (imgLabel->numOfPages())
//        return QPixmap(*imgLabel->pixmap());
    return QPixmap(); //fallback
}

int SimplePageView::viewWidth() const
{
    return width(); //TODO?
//    return (imgLabel->numOfPages()) ? imgLabel->width() : 0;
}

int SimplePageView::currentPage() const
{
    return imgLabel ? imgLabel->pageNumber() : -1;
}
