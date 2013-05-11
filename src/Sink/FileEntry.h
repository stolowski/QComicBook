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

#ifndef __FILE_ENTRY_H
#define __FILE_ENTRY_H

#include <QString>

class QFileInfo;

namespace QComicBook
{
    class FileEntry
    {
    private:
        QString m_root;
        QString m_fileName;     // just file name, e.g. "1.jpg"
        QString m_relativePath; // directory, relative to root dir of a comicbook

    public:
        const QString& getFileName() const;
        const QString& getRelativePath() const;
        const QString getFullFileName() const;

        FileEntry (const QString &root, const QFileInfo &finfo);
    };
}
 
#endif
