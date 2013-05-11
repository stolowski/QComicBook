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

#ifndef __DEBUG_CONTROLLER_H
#define __DEBUG_CONTROLLER_H

#include <QObject>
#include <QPointer>

class QTimer;

namespace QComicBook
{
    class PageViewBase;
    class ContinuousPageView;
    class ContinuousPageViewDebug;
    class MemoryDebug;
    
    class DebugController: public QObject
    {
        Q_OBJECT

    public:
        DebugController(QObject *parent);
        ~DebugController();

    public Q_SLOTS:
        void showDebugContinuousView();
        void showDebugMemory();
        void setView(PageViewBase *view);
        void autoRefreshEvery1sec();
        void autoRefreshEvery5sec();
        void autoRefreshEvery30sec();
        void disableAutoRefresh();
        
    private Q_SLOTS:
        void timerTimeout();
        void debugWindowDestroyed(QObject *object);

    private:
        ContinuousPageViewDebug *m_debugContinuousView;
        MemoryDebug *m_debugMemory;
        QTimer *m_timer;
        QPointer<PageViewBase> m_view;
    };
}

#endif
