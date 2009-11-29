/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

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
