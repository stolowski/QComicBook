#include "PageViewBase.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QBitmap>
#include <QCursor>
#include <QScrollBar>
#include <QPalette>

using namespace QComicBook;

PageViewBase::PageViewBase(QWidget *parent, bool twoPagesMode, Size size, const QColor &color)
    : QScrollArea(parent)
    , props(size, 0, twoPagesMode, false, true)
    , smallcursor(NULL)
{
    context_menu = new QMenu(this);
    connect(&props, SIGNAL(changed()), this, SLOT(propsChanged()));
}

PageViewBase::~PageViewBase()
{
}

void PageViewBase::scrollByDelta(int dx, int dy)
{
	QScrollBar *vbar = verticalScrollBar();
	QScrollBar *hbar = horizontalScrollBar();

	vbar->setValue(vbar->value() + dy);
	hbar->setValue(hbar->value() + dx);
}

void PageViewBase::contextMenuEvent(QContextMenuEvent *e)
{
    //if (imgs > 0)
    context_menu->popup(e->globalPos());
}

void PageViewBase::mouseMoveEvent(QMouseEvent *e)
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

void PageViewBase::mousePressEvent(QMouseEvent *e)
{
        if (!smallcursor)
                setCursor(Qt::PointingHandCursor);
}

void PageViewBase::mouseReleaseEvent(QMouseEvent *e)
{
        lx = -1;
	ly = -1;
        if (!smallcursor)
                setCursor(Qt::ArrowCursor);
}

void PageViewBase::mouseDoubleClickEvent(QMouseEvent *e)
{
	e->accept();
	emit doubleClick();
}

bool PageViewBase::onTop()
{
        return verticalScrollBar()->value() == verticalScrollBar()->minimum();
}

bool PageViewBase::onBottom()
{
        return verticalScrollBar()->value() == verticalScrollBar()->maximum();
}

void PageViewBase::setRotation(Rotation r)
{
    props.setAngle(r);
}

void PageViewBase::setSize(Size s)
{
    props.setSize(s);
}

void PageViewBase::setSizeOriginal()
{
        setSize(Original);
}

void PageViewBase::setSizeFitWidth()
{
        setSize(FitWidth);
}

void PageViewBase::setSizeFitHeight()
{
        setSize(FitHeight);
}

void PageViewBase::setSizeWholePage()
{
        setSize(WholePage);
}

void PageViewBase::setSizeBestFit()
{
        setSize(BestFit);
}

void PageViewBase::setBackground(const QColor &color)
{
    props.setBackground(color);
    QPalette palette;
    palette.setColor(backgroundRole(), color);
    setPalette(palette);
}

void PageViewBase::setTwoPagesMode(bool f)
{
    props.setTwoPagesMode(f);
}

void PageViewBase::setMangaMode(bool f)
{
    //TODO
}

void PageViewBase::scrollToTop()
{
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
}

void PageViewBase::scrollToBottom()
{
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void PageViewBase::setSmallCursor(bool f)
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

void PageViewBase::showPageNumbers(bool f)
{
    props.setPageNumbers(f);
}

void PageViewBase::enableScrollbars(bool f)
{
        const Qt::ScrollBarPolicy s = f ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff;
        setVerticalScrollBarPolicy(s);
        setHorizontalScrollBarPolicy(s);
}

QMenu *PageViewBase::contextMenu() const
{
        return context_menu;
}

ViewProperties& PageViewBase::properties()
{
    return props;
}

bool PageViewBase::hasRequest(int page) const
{
    return m_requestedPages.indexOf(page) >= 0;
}

void PageViewBase::addRequest(int page, bool twoPages)
{
    m_requestedPages.append(page);
    if (twoPages)
        emit requestTwoPages(page);
    else
        emit requestPage(page);
}

void PageViewBase::delRequest(int page, bool twoPages, bool cancel)
{
    int idx = m_requestedPages.indexOf(page);
    if (idx >= 0)
    {
        m_requestedPages.removeAt(idx);
        if (cancel)
        {
            if (twoPages)
                emit cancelTwoPagesRequest(page);
            else
                emit cancelPageRequest(page);
        }
    }
}

void PageViewBase::delRequests()
{
    m_requestedPages.clear();
}
