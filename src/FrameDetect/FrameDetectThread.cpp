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

#include <FrameDetectThread.h>
#include <FrameDetect.h>
#include <FrameCache.h>
#include "Page.h"
#include <QDebug>

using namespace QComicBook;
			
FrameDetectThread::FrameDetectThread()
	: QThread()
	, m_stop(false)
{

}

FrameDetectThread::~FrameDetectThread()
{

}

void FrameDetectThread::run()
{
    for (;;)
    {
        m_condMtx.lock();
        m_reqCond.wait(&m_condMtx);
        m_condMtx.unlock();

		for (;;)
		{
			m_processListMtx.lock();
			if (m_stop)
			{
				m_processListMtx.unlock();
				break;
			}
			Page p = m_pages.first();
			m_pages.pop_front();
			m_processListMtx.unlock();

			qDebug() << "FrameDetectThread: processing page" << p.getNumber();

			FrameCache &fc(FrameCache::instance());
			if (fc.has(p.getNumber()))
			{
				qDebug() << "frames for page" << p.getNumber() << "in cache";
				emit framesReady(fc.get(p.getNumber()));
			}
			else
			{
				FrameDetect fd(p);
				ComicFrameList frames(fd.process());
				fc.insert(frames);
				emit framesReady(frames);
			}

			m_processListMtx.lock();
			volatile int n = m_pages.count();
			m_processListMtx.unlock();
			if (n == 0)
			{
				break;
			}
		}
	}
}

void FrameDetectThread::clear()
{
	m_processListMtx.lock();
	m_pages.clear();
	m_processListMtx.unlock();
	FrameCache::instance().clear();
}

void FrameDetectThread::process(const Page &p)
{
	qDebug() << "FrameDetectThread: requested page" << p.getNumber();
	m_processListMtx.lock();
	m_pages.append(p);
	m_processListMtx.unlock();
	m_reqCond.wakeOne();
}

void FrameDetectThread::stop()
{
	m_processListMtx.lock();
	m_stop = true;
	m_processListMtx.unlock();
	m_reqCond.wakeOne();
}
