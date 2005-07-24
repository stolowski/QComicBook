/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

/*! \file imgcache.h */

#ifndef __IMGCACHE_H
#define __IMGCACHE_H

#include <qcache.h>
#include <qimage.h>

class ImgCache: public QCache<QImage>
{
        private:
                //! disables parent's find method
                QImage *find(const QString & k, bool ref = TRUE);
                
                //! disables parent's remove method
                bool remove(const QString & k);
                
                //! disables parent's take method
                QImage *take(const QString & k);
                               
        public:
                ImgCache(int size=1);
                virtual ~ImgCache();

                //! checks if image exists in cache
                virtual bool exists(const QString &key);
                
                //! returns a deep copy of image, if found
                virtual bool get(const QString &key, QImage &img);
                virtual void setSize(int size);
                
                //! overrides parent's method; ensures that a deep copy of image is inserted
                virtual QPtrCollection::Item newItem(QPtrCollection::Item d);
};

#endif

