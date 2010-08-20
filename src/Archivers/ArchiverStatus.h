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

#ifndef __ARCHIVER_STATUS_H
#define __ARCHIVER_STATUS_H

#include <QString>
#include <QList>
#include <QStringList>
#include "ArchiverHint.h"

namespace QComicBook
{
    class ArchiverStatus
    {
    public:
        ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QString &exec1, const QString &exec2=QString::null);
        ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QStringList &execlist);
        ~ArchiverStatus();

        bool isSupported() const;
        QString name() const;
        QStringList executables() const;
        QStringList extensions() const;

    private:
        QString m_name;
        QStringList m_executables;
        QStringList m_extensions;
        bool m_supported;
    };
}

#endif
