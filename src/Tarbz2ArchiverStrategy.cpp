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

#include "Tarbz2ArchiverStrategy.h"
#include "Utility.h"

using namespace QComicBook;
using Utility::which;

Tarbz2ArchiverStrategy::Tarbz2ArchiverStrategy()
    : ArchiverStrategy("tar.bz2", FileSignature())
{
}

Tarbz2ArchiverStrategy::~Tarbz2ArchiverStrategy()
{
}

void Tarbz2ArchiverStrategy::configure()
{
    addExtension(".tar.bz2");
    addExtension(".cbb");
    setExecutables("tar");

    if (which("tar") != QString::null)
    {
        setExtractArguments("tar -xvjf @F");
        setListArguments("tar -tjf @F");
        setSupported();
    }
}
