/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2006 Pawel Stolowski <pawel.stolowski@wp.pl>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __ENUMMAP_H
#define __ENUMMAP_H

#include <qstring.h>

namespace Utility
{
        template <class T> struct EnumMap
        {
                QString str;
                T val;
        };

        template <class T> const QString& convert(const EnumMap<T> *tab, const T &v) 
        {
                for (int i=0; tab[i].str!=QString::null; i++)
                        if (tab[i].val == v)
                                return tab[i].str;
                return tab[0].str;
        }

        template <class T> const T& convert(const EnumMap<T> *tab, const QString &s) 
        {
                for (int i=0; tab[i].str!=QString::null; i++)
                        if (tab[i].str == s)
                                return tab[i].val;
                return tab[0].val;
        }
}

#endif

