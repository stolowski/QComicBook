/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2011 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __IMAGEJOBRESULT_H
#define __IMAGEJOBRESULT_H

#include <QImage>
#include "JobKey.h"

namespace QComicBook
{
    struct ImageJobResult
    {
        const JobKey key;
        const QImage image;

        ImageJobResult() {}
        ImageJobResult(const JobKey &key, const QImage &img): key(key), image(img) {}
    };
}

#endif
