/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2013 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "FileEntry.h"
#include <QFileInfo>
#include <QDebug>

using namespace QComicBook;

FileEntry::FileEntry(const QString &root, const QFileInfo &finfo):
    m_root(root)
{
    m_fileName = finfo.fileName();
    if (!root.isEmpty() && finfo.absolutePath().startsWith(root))
    {
        m_relativePath = finfo.absolutePath().mid(root.length(), -1);
    }
    else
    {
        m_relativePath = finfo.absolutePath();
    }
    qDebug() << "File entry: " << m_fileName << m_relativePath;
}

const QString FileEntry::getFullFileName() const
{
    return m_root + "/" + m_relativePath + "/" + m_fileName;
}
