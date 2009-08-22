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

#include "P7zipArchiverStrategy.h"
#include "Utility.h"

using namespace QComicBook;
using Utility::which;

P7zipArchiverStrategy::P7zipArchiverStrategy()
    : ArchiverStrategy("p7zip", FileSignature())
{
}

P7zipArchiverStrategy::~P7zipArchiverStrategy()
{
}

void P7zipArchiverStrategy::configure()
{
    addExtension(".7z");

    if (which("7z") != QString::null)
    {
        setExtractArguments("7z x @F");
        setListArguments("7z l @F");
        setSupported();
    }
}
