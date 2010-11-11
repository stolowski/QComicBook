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

#include "PrinterThread.h"
#include "ImgDirSink.h"
#include <QPrinter>
#include <QPainter>

using namespace QComicBook;

PrinterThread::PrinterThread(ImgDirSink *sink, QSharedPointer<QPrinter> printer, QAbstractPrintDialog::PrintRange range, int from, int to)
    : QThread()
    , m_sink(sink)
    , m_printer(printer)
    , m_range(range)
    , m_from(from)
    , m_to(to)
    , m_abort(false)
{
    if (range == QAbstractPrintDialog::AllPages)
    {
        m_from = 1;
        m_to = sink->numOfImages();
    }
}

PrinterThread::~PrinterThread()
{
	m_printer.clear();
}

void PrinterThread::run()
{
    QPainter painter;

    painter.begin(m_printer.data());
    QRectF pageRect(m_printer->pageRect());

    for (int i=m_from-1; i<m_to; i++)
    {
        m_printMtx.lock();
        volatile bool f(m_abort);
        m_printMtx.unlock();
        if (f)
        {
            m_printer->abort();
            break;
        }
        int result;
        Page page = m_sink->getImage(i, result);
        // TODO check result
        emit printing(i);
        painter.drawImage(pageRect, page.getImage());
        if (i != m_to-1)
        {
            m_printer->newPage();
        }
    }
    painter.end();
}

void PrinterThread::abort()
{
    m_printMtx.lock();
    m_abort = true;
    m_printMtx.unlock();
}
