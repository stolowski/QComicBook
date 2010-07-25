/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __ARCHIVER_HINT_H
#define __ARCHIVER_HINT_H

#include <QString>

namespace QComicBook
{
    class ArchiverHint
    {
    public:
        enum Severity { Info, Warning, Error };

        ArchiverHint(const QString &message, Severity severity=Info);
        ~ArchiverHint();

        Severity getSeverity() const
        {
            return sev;
        }

        QString getMessage() const
        {
            return msg;
        }

    private:
        QString msg;
        Severity sev;
    };
}

#endif
