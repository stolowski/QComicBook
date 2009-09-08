#include "ViewProperties.h"

using namespace QComicBook;

ViewProperties::ViewProperties(Size size, unsigned char angle, bool twoPagesMode, bool pagenumbers, bool contScroll)
    : m_size(size)
    , m_angle(angle)
    , m_pageNumbers(pagenumbers)
    , m_contScroll(contScroll)
    , m_twoPagesMode(twoPagesMode)
{
}

int ViewProperties::angle() const
{
    return m_angle;
}

void ViewProperties::setAngle(Rotation r)
{
    if (r == None && m_angle == 0)
        return;
    
    if (r == Right)
        ++m_angle;
    else if (r == Left)
        --m_angle;
    else
        m_angle = 0; //None
    m_angle &= 3;
    emit changed();
}


Size ViewProperties::size() const
{
    return m_size;
}

void ViewProperties::setSize(Size s)
{
    if (m_size != s)
    {
        m_size = s;
        emit changed();
    }
}

bool ViewProperties::pageNumbers() const
{
    return m_pageNumbers;
}

void ViewProperties::setPageNumbers(bool f)
{
    if (m_pageNumbers != f)
    {
        m_pageNumbers = f;
        emit changed();
    }
}

QColor ViewProperties::background() const
{
    return m_background;
}

void ViewProperties::setBackground(const QColor &c)
{
    if (m_background != c)
    {
        m_background = c;
        emit changed();
    }
}

bool ViewProperties::continuousScrolling() const
{
    return m_contScroll;
}

void ViewProperties::setTwoPagesMode(bool f)
{
    if (m_twoPagesMode != f)
    {
        m_twoPagesMode = f;
        emit changed();
    }
}

bool ViewProperties::twoPagesMode() const
{
    return m_twoPagesMode;
}
