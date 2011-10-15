#ifndef __PAGEREDRAWJOB_H
#define __PAGEREDRAWJOB_H

#include "ImageTransformJob.h"
#include "../ViewPropertiesData.h"

class QImage;

namespace QComicBook
{
    class Page;

    class PageRedrawJob: public ImageTransformJob
    {
    public:
        PageRedrawJob();
        ~PageRedrawJob();

        void setImage(const Page &p1);
        void setImage(const Page &p1, const Page &p2);
        void setSourceSize(const QSize &size);
        void setViewProperties(const ViewPropertiesData &props);

        void execute();
        QImage getResult() const;

    protected:
        void drawPageNumber(int page, QPainter &p, int x, int y);

    private:
        QImage *m_image[2];
        int m_numbers[2];  //!< page numbers
        QImage *m_result; //!< resulting image
        QSize m_sourceSize;
        ViewPropertiesData m_props; //!< view properties
    };
}

#endif
