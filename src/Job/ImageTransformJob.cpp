#include "ImageTransformJob.h"
#include "ComicBookDebug.h"
#include <QMatrix>

using namespace QComicBook;

ImageTransformJob::ImageTransformJob(): m_key(-1), m_matrix(0)
{
}

ImageTransformJob::~ImageTransformJob()
{
    _DEBUG;
    delete m_matrix;
}

void ImageTransformJob::setKey(int k)
{
    m_key = k;
}

int ImageTransformJob::key() const
{
    return m_key;
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
