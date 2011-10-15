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

#ifndef __VIEWPROPERTIESDATA_H
#define __VIEWPROPERTIESDATA_H

#include <QColor>

namespace QComicBook
{
    enum Size { Original, FitWidth, FitHeight, WholePage, BestFit };
    enum Rotation { None, Left, Right };

    struct ViewPropertiesData
    {

        unsigned char angle; //rotation angle, 0..3, multipled by 90
        Size size;
        QColor background; //background color
        bool pageNumbers;  //if page numbers should be embedded
        bool twoPagesMode;
        bool mangaMode;
        bool contScroll;
    };
}

#endif
