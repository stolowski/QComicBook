#include "ImageTransformJob.h"
#include "ComicBookDebug.h"
#include <QMatrix>
#include <QImage>
#include <QPainter>

using namespace QComicBook;

ImageTransformJob::ImageTransformJob(): m_result(0), m_matrix(0)
{
    m_image[0] = m_image[1] = 0;
}

ImageTransformJob::~ImageTransformJob()
{
    _DEBUG;
    delete m_result;
    delete m_image[0];
    delete m_image[1];
}

void ImageTransformJob::setSize(int w, int h)
{
    m_width = w;
    m_height = h;
}

void ImageTransformJob::setMatrix(const QMatrix &m)
{
    delete m_matrix;
    m_matrix = new QMatrix(m);
}

void ImageTransformJob::setImage(const QImage &img1)
{
    m_image[0] = new QImage(img1);
}

void ImageTransformJob::setImage(const QImage &img1, const QImage &img2)
{
    m_image[0] = new QImage(img1);
    m_image[1] = new QImage(img2);
}

void ImageTransformJob::execute()
{
    _DEBUG;

    m_result = new QImage(m_width, m_height, QImage::Format_RGB32);

    QPainter p(m_result);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true /*ComicBookSettings::instance().smoothScaling()*/);

    p.setWorldMatrix(*m_matrix);
    p.setWorldMatrixEnabled(true);

    p.drawImage(0, 0, *m_image[0], 0, 0);
    p.end();
}

void ImageTransformJob::setKey(int k)
{
    m_key = k;
}

int ImageTransformJob::key() const
{
    return m_key;
}

QImage ImageTransformJob::getResult() const
{
    if (m_result)
    {
        return *m_result;
    }
    return QImage();
}
