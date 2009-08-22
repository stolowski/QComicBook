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

#include "ZipArchiverStrategy.h"
#include "Utility.h"

using namespace QComicBook;
using Utility::which;

ZipArchiverStrategy::ZipArchiverStrategy()
    : ArchiverStrategy("zip", FileSignature(0, "\x50\x4b\x03\x04", 4))
{
}

ZipArchiverStrategy::~ZipArchiverStrategy()
{
}

void ZipArchiverStrategy::configure()
{
    addExtension(".zip");
    addExtension(".cbz");

    if (which("unzip") != QString::null)
    {
        setExtractArguments("unzip @F");
        setListArguments("unzip -l @F");
        setSupported();
    }
}
