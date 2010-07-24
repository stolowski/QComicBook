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

#include "RecentFilesMenu.h"
#include "Utility.h"
#include <QStringList>

using namespace QComicBook;

RecentFilesMenu::RecentFilesMenu(const QString &title, QWidget *parent, int size)
    : QMenu(title, parent)
{
    maxSize = size;
    separator  = addSeparator();
    clearRecent = addAction(tr("Cleanup list"));

    connect(this, SIGNAL(triggered(QAction *)), this, SLOT(recentSelected(QAction *)));
}

RecentFilesMenu::~RecentFilesMenu()
{
}

void RecentFilesMenu::recentSelected(QAction *a)
{
    if (a == clearRecent)
    {
        emit cleanupRequest();
    }
    else
    {
        const QString fname(a->data().toString());
        emit selected(fname);
    }
}

void RecentFilesMenu::add(const QString &fname)
{
    //
    // try to remove the file from list first
    remove(fname);

    //
    // add the file as the first item on the list
    QAction *a = new QAction(Utility::shortenPath(fname, "...", 64), this);
    a->setData(fname);
    insertAction(actions().first(), a);

    
    //
    // remove actions if max number of actions is exceeded
    if (maxSize > 0)
    {
        int count = 0;
        foreach (QAction *a, actions())
        {
            if (a != clearRecent && !a->isSeparator())
            {
                ++count;
                if (count > maxSize)
                {
                    a->deleteLater();
                }
            }
        }
    }
}

void RecentFilesMenu::remove(const QString &fname)
{
    foreach (QAction *a, actions())
    {
        if (a != clearRecent && !a->isSeparator())
        {
            if (a->data().toString() == fname)
            {
                a->deleteLater();
                break;
            }
        }
    }
}

void RecentFilesMenu::removeAll()
{
    foreach (QAction *a, actions())
    {
        if (a != clearRecent && !a->isSeparator())
        {
            a->deleteLater();
        }
    }
}

void RecentFilesMenu::set(const QStringList &files)
{
    removeAll();

    foreach (const QString &fname, files)
    {
        QAction *a = new QAction(Utility::shortenPath(fname, "...", 64), this);
        a->setData(fname);
        insertAction(separator, a);
    }
}

QStringList RecentFilesMenu::get() const
{
    QStringList list;
    foreach (QAction *a, actions())
    {
        if (a != clearRecent && !a->isSeparator())
        {
            const QString fname(a->data().toString());
            list.append(fname);
        }
    }
    return list;
}
