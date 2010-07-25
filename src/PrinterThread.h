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

#ifndef __PRINTER_THREAD_H
#define __PRINTER_THREAD_H

#include <QThread>
#include <QAbstractPrintDialog>

class QPrinter;

namespace QComicBook
{
    class ImgDirSink;

    class PrinterThread: public QThread
    {
    Q_OBJECT

    public:
        PrinterThread(ImgDirSink *sink, QPrinter *printer, QAbstractPrintDialog::PrintRange range, int from, int to);
        ~PrinterThread();
        void run();

    signals:
        void printing(int n);

    private:
        QPrinter *m_printer;
        ImgDirSink *m_sink;
        QAbstractPrintDialog::PrintRange m_range;
        int m_from;
        int m_to;
    };
}

#endif
