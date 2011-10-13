#include <QImage>
#include <QPainter>
#include "../ComicBookDebug.h"
#include "PageRedrawJob.h"

using namespace QComicBook;

PageRedrawJob::PageRedrawJob(): ImageTransformJob(), m_result(0)
{
    m_image[0] = m_image[1] = 0;
}

PageRedrawJob::~PageRedrawJob()
{
    _DEBUG;
    delete m_result;
    delete m_image[0];
    delete m_image[1];
}

void PageRedrawJob::setImage(const QImage &img1)
{
    m_image[0] = new QImage(img1);
}

void PageRedrawJob::setImage(const QImage &img1, const QImage &img2)
{
    m_image[0] = new QImage(img1);
    m_image[1] = new QImage(img2);
}

void PageRedrawJob::execute()
{
    _DEBUG;

    delete m_result;
    m_result = new QImage(m_width, m_height, QImage::Format_RGB32);

    QPainter p(m_result);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true /*ComicBookSettings::instance().smoothScaling()*/);

    p.setWorldMatrix(*m_matrix);
    p.setWorldMatrixEnabled(true);

    p.drawImage(0, 0, *m_image[0], 0, 0);
    p.end();
}

QImage PageRedrawJob::getResult() const
{
    if (m_result)
    {
        return *m_result;
    }
    return QImage();
}
