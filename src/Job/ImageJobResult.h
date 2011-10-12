#ifndef __IMAGEJOBRESULT_H
#define __IMAGEJOBRESULT_H

#include <QImage>

namespace QComicBook
{
    struct ImageJobResult
    {
        const int key;
        const QImage image;

        ImageJobResult(): key(-1) {}
        ImageJobResult(int key, const QImage &img):key(key), image(img) {}
    };
}

#endif
