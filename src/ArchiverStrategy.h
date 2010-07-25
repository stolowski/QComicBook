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

#ifndef __ARCHIVER_STRATEGY
#define __ARCHIVER_STRATEGY

#include <QString>
#include <QStringList>
#include "FileSignature.h"
#include "ArchiverHint.h"
#include "ArchiverStatus.h"

class QFile;

namespace QComicBook
{
    class ArchiverStrategy
    {
    public:
        ArchiverStrategy(const QString &name, const FileSignature &sig);
        virtual ~ArchiverStrategy();

        virtual bool canOpen(QFile *f) const;
        virtual bool canOpen(const QString &filename) const;
        QStringList getExtractArguments(const QString &filename) const;
        QStringList getExtractArguments() const;
        QStringList getListArguments(const QString &filename) const;
        QStringList getListArguments() const;
        QStringList getExtensions() const;
        bool isSupported() const;
        static QStringList fillTemplateArguments(const QStringList &args, const QString &filename);
        virtual void configure() = 0;
        virtual QList<ArchiverHint> getHints() const;

        operator ArchiverStatus() const;

    protected:
        void setFileMagic(const FileSignature &sig);
        void setExtractArguments(const QString &command);
        void setListArguments(const QString &command);
        void setSupported(bool f=true);
        void setExecutables(const QString &exec1, const QString &exec2=QString::null);
        void addExtension(const QString &ext);

    private:
        FileSignature signature;
        QString name;
        bool supported;
        QStringList executables;
        QStringList extensions;
        QStringList extractArgs;
        QStringList listArgs;
    };
}

#endif
