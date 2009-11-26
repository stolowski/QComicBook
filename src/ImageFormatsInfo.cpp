#include "ImageFormatsInfo.h"
#include <QImageReader>

using namespace QComicBook;

ImageFormatsInfo* ImageFormatsInfo::sm_info(NULL);

ImageFormatsInfo& ImageFormatsInfo::instance()
{
    if (!sm_info)
    {
        sm_info = new ImageFormatsInfo();
    }
    return *sm_info;
}

ImageFormatsInfo::ImageFormatsInfo()
{
    foreach (QByteArray b, QImageReader::supportedImageFormats())
    {
        //
        // Qt may support more formats, but this is to ensure only the ones that make sense for
        // QComicBook are included.
        if (b == "jpg" || b == "jpeg" || b == "bmp" || b == "png" || b == "tiff" || b == "gif" || b == "xpm")
        {
            m_extensions.append("." + b);
            m_formats.append(QString(b).toUpper());
        }
    }
}

ImageFormatsInfo::~ImageFormatsInfo()
{
    delete sm_info;
}

QStringList ImageFormatsInfo::extensions() const
{
    return m_extensions;
}

QStringList ImageFormatsInfo::formats() const
{
    return m_formats;
}
