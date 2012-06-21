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

#ifndef __IMAGETRANSFORMJOB_H
#define __IMAGETRANSFORMJOB_H

#include "JobKey.h"
#include "../ViewPropertiesData.h"
#include "Counted.h"

class QImage;
class QMatrix;

namespace QComicBook
{
    class ImageTransformJob: public Counted<ImageTransformJob>
    {
    public:
        ImageTransformJob();
        virtual ~ImageTransformJob();

        void setSize(int w, int h);
        void setMatrix(const QMatrix &m);
        void setKey(const JobKey &k);
        const JobKey& key() const;

        void setViewProperties(const ViewPropertiesData &props);

        virtual void execute() = 0;
        virtual QImage getResult() const = 0;

    protected:
        JobKey m_key;
        int m_width;
        int m_height;
        ViewPropertiesData m_props; //!< view properties
        QMatrix *m_matrix;
    };
}

#endif
