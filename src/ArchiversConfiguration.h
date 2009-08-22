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

#ifndef __ARCHIVERS_CONFIGURATION_H
#define __ARCHIVERS_CONFIGURATION_H

#include <QList>
#include <QStringList>

namespace QComicBook
{
    class ArchiverStrategy;

    class ArchiversConfiguration
    {
    public:
        static ArchiversConfiguration& instance();
        void getExtractArguments(const QString &filename, QStringList &extract, QStringList &list) const;
        QStringList getExtractArguments(const QString &filename) const;
        QStringList getListArguments(const QString &filename) const;
        QStringList supportedOpenExtensions() const;

    private:
        ArchiversConfiguration();
        ~ArchiversConfiguration();
        ArchiverStrategy* findStrategy(const QString &filename) const;

        static ArchiversConfiguration *cfg;
        QList<ArchiverStrategy *> archivers;
    };
}

#endif
