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

#include "AceArchiverStrategy.h"
#include "Utility.h"

using namespace QComicBook;
using Utility::which;

AceArchiverStrategy::AceArchiverStrategy()
    : ArchiverStrategy("ace", FileSignature(8, "\x2a\x41\x43\x45", 4))
{
}

AceArchiverStrategy::~AceArchiverStrategy()
{
}

void AceArchiverStrategy::configure()
{
    addExtension(".ace");
    addExtension(".cba");
    setExecutables("unace");

    if (which("unace") != QString::null)
    {
        setExtractArguments("unace x -y -c- @F");
        setListArguments("unace l -y -c- @F");
        setSupported();
    }
}
