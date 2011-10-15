#include <QImage>
#include <QPainter>
#include "../ComicBookDebug.h"
#include <algorithm>
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

    if (m_image[1]) // 2 pages mode
    {
	const int swap(m_props.mangaMode);
        // clear areas not covered by page (if pages sizes differ)
        for (int i=0; i<2; i++)
        {
            const int j(i^swap);
            if (m_image[j]->height() < std::max(m_image[0]->height(), m_image[1]->height()))
            {
                p.fillRect(i*m_image[j]->width(), m_image[j]->height(), m_image[j]->width(), m_sourceSize.height() - m_image[j]->height(), m_props.background);
                break; //only one page may be smaller
            }
        }

        p.drawImage(0, 0, *m_image[0^swap], 0, 0);
        p.drawImage(m_image[0^swap]->width(), 0, *m_image[1^swap], 0, 0);
        if (m_props.pageNumbers)
        {
            p.setWorldMatrixEnabled(false);
            // drawPageNumber(std::max(m_image[swap]->getNumber(), m_image[1^swap]->getNumber()), p, getScaledSize().width(), getScaledSize().height());
        }
    }
    else // 1 page mode
    {
        p.drawImage(0, 0, *m_image[0], 0, 0);
    }
    p.end();
}

void PageRedrawJob::setViewProperties(const ViewPropertiesData &props)
{
    m_props = props;
}

void PageRedrawJob::setSourceSize(const QSize &size)
{
    m_sourceSize = size;
}

QImage PageRedrawJob::getResult() const
{
    if (m_result)
    {
        return *m_result;
    }
    return QImage();
}
