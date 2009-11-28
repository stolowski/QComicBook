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

#include "FileSignature.h"

using namespace QComicBook;

FileSignature::FileSignature(unsigned int offset, const char *pattern, unsigned int len)
    : offset(offset)
{
    this->pattern = QByteArray(pattern, len);
}

FileSignature::FileSignature()
    : offset(0)
{
}

FileSignature::FileSignature(const FileSignature &sig)
{
    pattern = sig.pattern;
    offset = sig.offset;
}

FileSignature::~FileSignature()
{
}

bool FileSignature::matches(QFile *file) const
{
    if (pattern.size() > 0 && file->seek(offset))
    {
        for (int i=0; i<pattern.size(); i++)
        {
            char c;
            if (!file->getChar(&c))
                return false;
            if (c != pattern.at(i))
                return false;
        }
        return true;
    }
    return false;
}

FileSignature& FileSignature::operator =(const FileSignature &sig)
{
    if (this != &sig)
    {
        pattern = sig.pattern;
        offset = sig.offset;
    }
    return *this;
}
