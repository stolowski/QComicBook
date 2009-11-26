#ifndef __IMAGE_FORMATS_INFO_H
#define __IMAGE_FORMATS_INFO_H

#include <QStringList>

namespace QComicBook
{
    class ImageFormatsInfo
    {
    public:
        static ImageFormatsInfo& instance();

        QStringList extensions() const;
        QStringList formats() const;

    private:
        ImageFormatsInfo();
        ImageFormatsInfo(const ImageFormatsInfo &);
        ~ImageFormatsInfo();

        static ImageFormatsInfo *sm_info;
        QStringList m_extensions;
        QStringList m_formats;
    };
}

#endif
