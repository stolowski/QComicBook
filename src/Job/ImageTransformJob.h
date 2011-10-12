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
        ~ImageTransformJob();

        void execute();

        int key() const;

        void setSize(int w, int h);
        void setMatrix(const QMatrix &m);
        void setImage(const QImage &img1);
        void setImage(const QImage &img1, const QImage &img2);

        QImage getResult() const;

    private:
        int m_width;
        int m_height;
        QImage *m_image[2];
        QImage *m_result;
        QMatrix *m_matrix;
    };
}

#endif
