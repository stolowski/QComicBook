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

#ifndef __FRAMEREDRAWJOB_H
#define __FRAMEREDRAWJOB_H

#include "ImageTransformJob.h"
#include <QRect>

namespace QComicBook
{
    class FrameRedrawJob: public ImageTransformJob
    {
    public:
        FrameRedrawJob();
        ~FrameRedrawJob();

        void setImage(const QImage &img, const QRect &frame);
        
        void execute();
        QImage getResult() const;

    private:
        QImage *m_img;
        QRect m_rect;
        QImage *m_result;
    };
}

#endif
