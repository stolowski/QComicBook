#ifndef __PAGEREDRAWJOB_H
#define __PAGEREDRAWJOB_H

#include "ImageTransformJob.h"
#include "../ViewPropertiesData.h"

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
        void setSourceSize(const QSize &size);
        void setViewProperties(const ViewPropertiesData &props);

        void execute();
        QImage getResult() const;

    protected:
        QImage *m_image[2];
        QImage *m_result;
        QSize m_sourceSize;
        ViewPropertiesData m_props;
    };
}

#endif
