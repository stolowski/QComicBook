/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __FILE_SIGNATURE_H
#define __FILE_SIGNATURE_H

#include <QFile>
#include <QByteArray>

namespace QComicBook
{
    class FileSignature
    {
    public:
        FileSignature(unsigned int offset, const char *pattern, unsigned int len);
        FileSignature();
        FileSignature(const FileSignature &sig);
        ~FileSignature();

        bool matches(QFile *file) const;
        FileSignature& operator =(const FileSignature &sig);

    private:
        unsigned int offset;
        QByteArray pattern;
    };
}

#endif
