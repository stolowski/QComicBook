/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2012 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __COORDINATE_RANGE_LIST_H
#define __COORDINATE_RANGE_LIST_H

#include <QVector>

namespace QComicBook
{
    template <typename T>
    class CoordinateRangeList
    {
        public:
            CoordinateRangeList();
            ~CoordinateRangeList();

            void reset();
            void resize(int n);
            T& startCoordinate(unsigned int index);
            const T& startCoordinate(unsigned int index) const;
            T& endCoordinate(unsigned int index);
            const T& endCoordinate(unsigned int index) const;

        private:
            QVector<T> m_start;
            QVector<T> m_end;
    };

    template <typename T>
    CoordinateRangeList<T>::CoordinateRangeList()
    {
    }

    template <typename T>
    CoordinateRangeList<T>::~CoordinateRangeList()
    {
    }

    template <typename T>
    void CoordinateRangeList<T>::reset()
    {
        m_start.clear();
        m_end.clear();
    }
    
    template <typename T>
    void CoordinateRangeList<T>::resize(int n)
    {
        m_start.resize(n);
        m_end.resize(n);
    }
    
    template <typename T>
    T& CoordinateRangeList<T>::startCoordinate(unsigned int index)
    {
        return m_start[index];
    }
    
    template <typename T>
    const T& CoordinateRangeList<T>::startCoordinate(unsigned int index) const
    {
        return m_start.at(index);
    }

    template <typename T>
    T& CoordinateRangeList<T>::endCoordinate(unsigned int index)
    {
        return m_end[index];
    }
    
    template <typename T>
    const T& CoordinateRangeList<T>::endCoordinate(unsigned int index) const
    {
        return m_end.at(index);
    }
}


#endif
