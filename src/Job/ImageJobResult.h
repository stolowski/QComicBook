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
