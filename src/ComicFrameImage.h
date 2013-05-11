/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __COMIC_FRAME_IMAGE_H
#define __COMIC_FRAME_IMAGE_H

#include "ComicImage.h"
#include <QRect>

class QImage;

namespace QComicBook
{
	class Page;
	class ComicFrame;
	class FrameView;

	class ComicFrameImage: public ComicImage
	{
        public:
            ComicFrameImage(FrameView *parent, int w, int h);
            virtual ~ComicFrameImage();
            
            void setFrame(const Page &p, const ComicFrame &f);
            void clear();
            ImageTransformJob *createRedrawJob();
            bool jobCompleted(const ImageJobResult &result);
            void propsChanged();
            
        private:
            QImage *m_image;
            QRect m_frame;
            int m_framekey;
	};
}

#endif
