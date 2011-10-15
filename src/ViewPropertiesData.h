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
