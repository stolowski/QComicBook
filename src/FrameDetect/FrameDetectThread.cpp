#include <FrameDetectThread.h>
#include <FrameDetect.h>
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

		for (;;) //TODO stop
		{
			m_processListMtx.lock();
			Page p = m_pages.first();
			m_pages.pop_front();
			m_processListMtx.unlock();

			qDebug() << "FrameDetectThread: processing page" << p.getNumber();

			FrameDetect fd(p.getImage());
			fd.process();
			emit framesReady(p.getNumber(), fd.frames());

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
	//TODO
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
}
