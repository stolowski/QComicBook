#ifndef __VIEW_PROPERTIES_H
#define __VIEW_PROPERTIES_H

#include <QObject>
#include <QMatrix>
#include <QColor>

namespace QComicBook
{
    enum Size { Original, FitWidth, FitHeight, WholePage, BestFit };
    enum Rotation { None, Left, Right };

    class ViewProperties: public QObject
    {
    Q_OBJECT

    signals:
        void changed();

    public:
        ViewProperties(Size size, unsigned char angle, bool pagenumbers, bool contScroll);

        int angle() const;
        void setAngle(Rotation r);
        Size size() const;
        void setSize(Size s);
        bool pageNumbers() const;
        void setPageNumbers(bool f);
        QColor background() const;
        void setBackground(const QColor &c);
        bool continuousScrolling() const;

    private:
        unsigned char m_angle; //rotation angle, 0..3, multipled by 90
        Size m_size;
        QMatrix m_rmatrix;  //rotation matrix
        QColor m_background; //background color
        bool m_pageNumbers;  //if page numbers should be embedded
        bool m_contScroll;

        friend class ComicImageView;
    };
}

#endif
