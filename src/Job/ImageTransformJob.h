#ifndef __IMAGETRANSFORMJOB_H
#define __IMAGETRANSFORMJOB_H

class QImage;
class QMatrix;

namespace QComicBook
{
    class ImageTransformJob
    {
    public:
        ImageTransformJob();
        virtual ~ImageTransformJob();

        void setSize(int w, int h);
        void setMatrix(const QMatrix &m);
        void setKey(int k);
        int key() const;

        virtual void execute() = 0;
        virtual QImage getResult() const = 0;

    protected:
        int m_key;
        int m_width;
        int m_height;
        QMatrix *m_matrix;
    };
}

#endif
