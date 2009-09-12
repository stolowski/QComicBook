#include "SimplePageView.h"

#include "ContinuousPageView.h"
#include "Utility.h"
#include "PageWidget.h"
#include <QVBoxLayout>
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
    , imgLabel(NULL)
{
    //setFocusPolicy(QWidget::StrongFocus);
    QWidget *w = new QWidget(this);
    w->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    m_layout = new QVBoxLayout(w);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setAlignment(Qt::AlignCenter);
    setWidget(w);

    recreatePageWidget();
    setWidgetResizable(false);
    
    setWidgetResizable(true);
    
    setBackground(color);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

SimplePageView::~SimplePageView()
{
}

void SimplePageView::recreatePageWidget()
{
    delete imgLabel;
    imgLabel = NULL;

    int w = viewport()->width() - 10;
    int h = viewport()->height() - 10;

    if (m_physicalPages)
    {
        imgLabel = new PageWidget(this, w, h, 0, props.twoPagesMode());
        m_layout->addWidget(imgLabel); 
    }
}

void SimplePageView::setNumOfPages(int n)
{
    m_physicalPages = n;
    recreatePageWidget();
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
    if (img1.getNumber() == m_currentPage)
    {
        imgLabel->setImage(img1);
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);        
    }
}

void SimplePageView::setImage(const Page &img1, const Page &img2)
{
    Q_ASSERT(m_physicalPages > 0);

    //  if (!preserveangle)
    //          iangle = 0;
    if (img1.getNumber() == m_currentPage)
    {
        imgLabel->setImage(img1, img2);
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);        
    }
}

void SimplePageView::gotoPage(int n)
{
    if (n < m_physicalPages)
    {
        if (n != m_currentPage)
        {
            //delRequest(m_currentPage);
        }
        if (props.twoPagesMode() && m_physicalPages % 2 == 0)
        {
            n -= n & 1;
        }
        m_currentPage = n;
        addRequest(m_currentPage, props.twoPagesMode() && !((m_physicalPages % 2 !=0 ) && n == 0));
        emit currentPageChanged(n);
    }
}

void SimplePageView::resizeEvent(QResizeEvent *e)
{
    if (imgLabel)
    {
        imgLabel->redrawImages();
    }
    QScrollArea::resizeEvent(e);
}

void SimplePageView::wheelEvent(QWheelEvent *e)
{
    
    if (e->delta() > 0) //scrolling up
    {
        if (imgLabel->height() <= height() || (onTop() && ++wheelupcnt > EXTRA_WHEEL_SPIN))
        {
            e->accept();
            wheelupcnt = 0;
            gotoPage(m_currentPage - 1);
        }
        else
        {
            QScrollArea::wheelEvent(e);
            wheeldowncnt = 0; //reset opposite direction counter
        }
    }
    else //scrolling down
    {
        if (imgLabel->height() <= height() || (onBottom() && ++wheeldowncnt > EXTRA_WHEEL_SPIN))
        {
            e->accept();
            wheeldowncnt = 0;
            gotoPage(m_currentPage + 1);
        }
        else
        {
            QScrollArea::wheelEvent(e);
            wheelupcnt = 0; //reset opposite direction counter
        }
    }
}


void SimplePageView::setTwoPagesMode(bool f)
{
    /*if (f != m_twoPagesMode)
    {
        m_twoPagesMode = f;
        recreatePageWidgets();
        }*/
    recreatePageWidget();
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
