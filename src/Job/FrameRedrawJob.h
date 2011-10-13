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
