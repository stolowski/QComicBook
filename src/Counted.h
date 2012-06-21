#ifndef __COUNTED_H
#define __COUNTED_H

namespace QComicBook
{
    template <typename T>
        class Counted
    {
    public:
        Counted()
        {
            ++sm_count;
            ++sm_total;
        }

        virtual ~Counted()
        {
            --sm_count;
        }

        static int objectCount()
        {
            return sm_count;
        }

        static int objectTotal()
        {
            return sm_total;
        }

    private:
        static int sm_count;
        static int sm_total;
    };

    template <typename T>
        int Counted<T>::sm_count = 0;

    template <typename T>
        int Counted<T>::sm_total = 0;
}

#endif
