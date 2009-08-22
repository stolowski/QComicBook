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

#include "RarArchiverStrategy.h"
#include "Utility.h"
#include <QTextStream>

using namespace QComicBook;
using Utility::which;

RarArchiverStrategy::RarArchiverStrategy()
    : ArchiverStrategy("rar", FileSignature(0, "\x52\x61\x72\x21", 4))
{
}

RarArchiverStrategy::~RarArchiverStrategy()
{
}

void RarArchiverStrategy::configure()
{
    addExtension(".rar");
    addExtension(".cbr");

    if (which("rar") != QString::null)
    {
        setExtractArguments("rar x @F");
        setListArguments("rar lb @F");
        setSupported();
    }
    else if (which("unrar") != QString::null)
    {
        FILE *f;
        bool nonfree_unrar = false;
        //
        // now determine which unrar it is - free or non-free
        if ((f = popen("unrar", "r")) != NULL)
        {
            QRegExp regexp("^UNRAR.+freeware");
            for (QTextStream s(f); !s.atEnd(); )
            {
                if (regexp.indexIn(s.readLine()) >= 0)
                {
                    nonfree_unrar = true;
                    break;
                }
            }
            pclose(f);
            if (nonfree_unrar)
            {
                setExtractArguments("unrar x @F");
                setListArguments("unrar lb @F");
            }
            else
            {
                setExtractArguments("unrar -x @F");
                setListArguments("unrar -t @F");
            }
            setSupported();
        }
    }
    else if (which("unrar-free") != QString::null) //some distros rename free unrar like this
    {
        setExtractArguments("unrar-free -x @F");
        setListArguments("unrar-free -t @F");
        setSupported();
    }
}
