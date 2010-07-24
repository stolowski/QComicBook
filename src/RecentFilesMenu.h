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

#ifndef __RECENT_FILES_MENU_H
#define __RECENT_FILES_MENU_H

#include <QMenu>

class QStringList;
class QAction;

namespace QComicBook
{
    class RecentFilesMenu: public QMenu
    {
    Q_OBJECT
    public:
        RecentFilesMenu(const QString &title, QWidget *parent, int size=0);
        ~RecentFilesMenu();

    public slots:
        void set(const QStringList &files);
        QStringList get() const;
        void add(const QString &fname);
        void remove(const QString &fname);
        void removeAll();

    protected slots:
        void recentSelected(QAction *a);
 
    signals:
        void selected(const QString &fileName);
        void cleanupRequest();

    private:
        QAction *clearRecent;
        QAction *separator;
        int maxSize;
    };
}

#endif
