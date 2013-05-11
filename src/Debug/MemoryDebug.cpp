/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2012 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "MemoryDebug.h"
#include "ComicImage.h"
 
namespace QComicBook
{

MemoryDebug::MemoryDebug(QWidget *parent):
    QWidget(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

MemoryDebug::~MemoryDebug()
{
}

void MemoryDebug::refreshClicked()
{
    refresh();
}

void MemoryDebug::refresh()
{
    debug_text->clear();
    appendObjectCount("ComicImage", Counted<ComicImage>::objectCount(), Counted<ComicImage>::objectTotal());
    appendObjectCount("ImageTransformJob", Counted<ImageTransformJob>::objectCount(), Counted<ImageTransformJob>::objectTotal());
}

void MemoryDebug::appendObjectCount(const QString &className, int count, int total)
{
    debug_text->appendPlainText(QString("%1\t%2\t %3").arg(className, QString::number(count), QString::number(total)));
}

}
