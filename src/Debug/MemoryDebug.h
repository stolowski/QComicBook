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

#ifndef __MEMORY_DEBUG_H
#define __MEMORY_DEBUG_H

#include <QWidget>
#include "../ui_MemoryDebug.h"

namespace QComicBook
{
    class MemoryDebug: public QWidget, private Ui::MemoryDebug
    {
        Q_OBJECT

        public:
            MemoryDebug(QWidget *parent=0);
            ~MemoryDebug();

        public Q_SLOTS:
            void refresh();

        private Q_SLOTS:
            void refreshClicked();
          
        private:
            void appendObjectCount(const QString &className, int count, int total);
    };
}

#endif
