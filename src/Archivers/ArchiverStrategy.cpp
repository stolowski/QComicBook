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

#include "ArchiverStrategy.h"
#include <QFile>
#include <QDebug>

using namespace QComicBook;

ArchiverStrategy::ArchiverStrategy(const QString &name, const FileSignature &sig)
    : name(name)
    , supported(false)
    , signature(sig)
{
}

ArchiverStrategy::~ArchiverStrategy()
{
}

void ArchiverStrategy::setSupported(bool f)
{
    supported = f;
}

void ArchiverStrategy::addExtension(const QString &ext)
{
    if (extensions.indexOf(ext) < 0)
    {
        extensions.append(ext);
    }
}

void ArchiverStrategy::setFileMagic(const FileSignature &sig)
{
    signature = sig;
}

QStringList ArchiverStrategy::fillTemplateArguments(const QStringList & inargs, const QString &filename)
{
    QStringList args;
    foreach (QString s, inargs)
    {
        if (s == "@F")
        {
            args << filename;
        }
        else
        {
            args << s;
        }
    }
    return args;
}

void ArchiverStrategy::setExtractArguments(const QString &command)
{
    extractArgs = command.split(" ", QString::SkipEmptyParts);
}

void ArchiverStrategy::setListArguments(const QString &command)
{
    listArgs = command.split(" ", QString::SkipEmptyParts);
}

void ArchiverStrategy::setExecutables(const QString &exec1, const QString &exec2)
{
    executables.clear();
    executables.append(exec1);
    if (exec2 != QString::null)
    {
        executables.append(exec2);
    }
}

QStringList ArchiverStrategy::getExtractArguments(const QString &filename) const
{
    return fillTemplateArguments(extractArgs, filename);
}

QStringList ArchiverStrategy::getExtractArguments() const
{
    return extractArgs;
}

QStringList ArchiverStrategy::getListArguments(const QString &filename) const
{
    return fillTemplateArguments(listArgs, filename);
}

QStringList ArchiverStrategy::getListArguments() const
{
    return listArgs;
}

QStringList ArchiverStrategy::getExtensions() const
{
    return extensions;
}

bool ArchiverStrategy::canOpen(QFile *f) const
{
    return supported && signature.matches(f);
}

bool ArchiverStrategy::canOpen(const QString &filename) const
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        return canOpen(&file);
    }
}

QList<ArchiverHint> ArchiverStrategy::getHints() const
{
    return QList<ArchiverHint>(); // no hints by default
}

bool ArchiverStrategy::isSupported() const
{
    return supported;
}

ArchiverStrategy::operator ArchiverStatus() const
{
    Q_ASSERT(executables.size() > 0);
    return ArchiverStatus(supported, name, extensions, executables);
}
