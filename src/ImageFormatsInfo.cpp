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
        if (b == "jpg" || b == "jpeg" || b == "bmp" || b == "png" || b == "tiff" || b == "gif" || b == "xpm" ||
	    b == "heic" || b == "webp" )
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
