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

SimplePageView::SimplePageView(QWidget *parent, int physicalPages, const ViewProperties& props)
    : PageViewBase(parent, physicalPages, props)
    , wheelupcnt(0), wheeldowncnt(0)
    , m_currentPage(0) //??
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
    
    setWidgetResizable(true);
    
    setBackground(props.background());
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

    if (numOfPages())
    {
        imgLabel = new PageWidget(this, w, h, 0, props.twoPagesMode());
        m_layout->addWidget(imgLabel); 
    }
}

void SimplePageView::setNumOfPages(int n)
{
    PageViewBase::setNumOfPages(n);
    recreatePageWidget();
}

void SimplePageView::propsChanged()
{
    qDebug() << "SimplePageView::propsChanged()";
    if (imgLabel)
    {
        if ((props.twoPagesMode() && !imgLabel->hasTwoPages()) || (imgLabel->hasTwoPages() && !props.twoPagesMode()))
        {
            recreatePageWidget();
        }
        imgLabel->redrawImages();
        update();
        gotoPage(m_currentPage);
    }    
}

void SimplePageView::scrollContentsBy(int dx, int dy)
{
    PageViewBase::scrollContentsBy(dx, dy);
}

void SimplePageView::setImage(const Page &img1)
{
    Q_ASSERT(numOfPages() > 0);
    if (img1.getNumber() == m_currentPage)
    {
        imgLabel->setImage(img1);
        horizontalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
    }
}

void SimplePageView::setImage(const Page &img1, const Page &img2)
{
    Q_ASSERT(numOfPages() > 0);
    if (img1.getNumber() == m_currentPage)
    {
        imgLabel->setImage(img1, img2);
        horizontalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);        
    }
}

void SimplePageView::gotoPage(int n)
{
    if (n>= 0 && n < numOfPages())
    {
        if (n != m_currentPage)
        {
            //delRequest(m_currentPage);
        }
        if (props.twoPagesMode() && numOfPages() % 2 == 0 && props.twoPagesStep())
        {
            n -= n & 1;
        }
        m_currentPage = n;
        addRequest(m_currentPage, props.twoPagesMode() && !((numOfPages() % 2 !=0 ) && n == 0));
        emit currentPageChanged(n);
    }
}

void SimplePageView::resizeEvent(QResizeEvent *e)
{
    if (imgLabel)
    {
        imgLabel->redrawImages();
    }
    PageViewBase::resizeEvent(e);
}

void SimplePageView::wheelEvent(QWheelEvent *e)
{
    
    if (e->delta() > 0) //scrolling up
    {
        if (imgLabel->height() <= height() || (onTop() && ++wheelupcnt > EXTRA_WHEEL_SPIN))
        {
            e->accept();
            wheelupcnt = 0;
            gotoPage(previousPage(m_currentPage));
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
            gotoPage(nextPage(m_currentPage));
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
    recreatePageWidget(); //??
    props.setTwoPagesMode(f);
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
    delRequests();
    setNumOfPages(0);
}
			
int SimplePageView::visiblePages() const
{
    return imgLabel ? imgLabel->numOfPages() : 0;
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

