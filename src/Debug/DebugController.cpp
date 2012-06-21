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

#include "DebugController.h"
#include "View/ContinuousPageView.h"
#include "ContinuousPageViewDebug.h"
#include "MemoryDebug.h"
#include "ComicBookDebug.h"
#include <QTimer>

namespace QComicBook {

DebugController::DebugController(QObject *parent):
    m_debugContinuousView(NULL),
    m_debugMemory(NULL)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
}

DebugController::~DebugController()
{
    delete m_debugMemory;
    delete m_debugContinuousView;
}

void DebugController::setView(PageViewBase *view)
{
    m_view = QPointer<PageViewBase>(view);
    if (m_debugContinuousView) {
        m_debugContinuousView->setView(dynamic_cast<ContinuousPageView *>(m_view.data()));
    }
}

void DebugController::showDebugContinuousView()
{
    QComicBookDebug(
        delete m_debugContinuousView;
        m_debugContinuousView = new ContinuousPageViewDebug();
        connect(m_debugContinuousView, SIGNAL(destroyed(QObject *)), this, SLOT(debugWindowDestroyed(QObject *)));
        if (m_view) {
            m_debugContinuousView->setView(dynamic_cast<ContinuousPageView *>(m_view.data()));
        }
        m_debugContinuousView->show();
    );
}

void DebugController::showDebugMemory()
{
    QComicBookDebug(
        delete m_debugMemory;
        m_debugMemory = new MemoryDebug();
        connect(m_debugMemory, SIGNAL(destroyed(QObject *)), this, SLOT(debugWindowDestroyed(QObject *)));
        m_debugMemory->show();
    );
}

void DebugController::debugWindowDestroyed(QObject *object)
{
    if (object == m_debugMemory) {
        m_debugMemory = 0;
    } else if (object == m_debugContinuousView) {
        m_debugContinuousView = 0;
    } else {
        qCritical() << "Debug window destroyed, but not handled";
    }
}

void DebugController::autoRefreshEvery1sec()
{
    m_timer->start(1 * 1000);
}

void DebugController::autoRefreshEvery5sec()
{
     m_timer->start(5 * 1000);
}

void DebugController::autoRefreshEvery30sec()
{
    m_timer->start(30 * 1000);
}

void DebugController::disableAutoRefresh()
{
    m_timer->stop();
}
    
void DebugController::timerTimeout()
{
    if (m_debugMemory) {
        m_debugMemory->refresh();
    }
    if (m_debugContinuousView) {
        m_debugContinuousView->refresh();
    }
}

}
