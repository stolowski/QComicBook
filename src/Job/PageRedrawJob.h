#ifndef __PAGEREDRAWJOB_H
#define __PAGEREDRAWJOB_H

#include "ImageTransformJob.h"

class QImage;

namespace QComicBook
{
    class PageRedrawJob: public ImageTransformJob
    {
    public:
        PageRedrawJob();
        ~PageRedrawJob();

        void setImage(const QImage &img1);
        void setImage(const QImage &img1, const QImage &img2);

        void execute();
        QImage getResult() const;

    protected:
        QImage *m_image[2];
        QImage *m_result;
    };
}

#endif
