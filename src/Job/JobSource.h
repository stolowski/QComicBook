/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2012 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __JOB_SOURCE_H
#define __JOB_SOURCE_H

namespace QComicBook
{
    class ImageJobResult;
    class ImageTransformJob;

    class JobSource
    {
        public:
            virtual bool jobCompleted(const ImageJobResult &result) = 0;

        protected:
            virtual ImageTransformJob* createRedrawJob() = 0;
    };
}

#endif
