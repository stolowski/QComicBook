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
#include <QMutex>
#include <QAbstractPrintDialog>
#include <QSharedPointer>

class QPrinter;

namespace QComicBook
{
    class ImgSink;

    class PrinterThread: public QThread
    {
    Q_OBJECT

    public:
        PrinterThread(QSharedPointer<ImgSink> sink, QSharedPointer<QPrinter> printer, QAbstractPrintDialog::PrintRange range, int from, int to);
        ~PrinterThread();
        void run();

   public slots:
        void abort();

    signals:
        void printing(int n);

    private:
        QMutex m_printMtx;
        bool m_abort;
        QSharedPointer<QPrinter> m_printer;
        QSharedPointer<ImgSink> m_sink;
        QAbstractPrintDialog::PrintRange m_range;
        int m_from;
        int m_to;
    };
}

#endif
