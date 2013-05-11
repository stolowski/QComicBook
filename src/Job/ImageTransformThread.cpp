/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2011 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ImageTransformThread.h"
#include "ImageTransformJob.h"
#include "ImageJobResult.h"
#include "ComicBookDebug.h"

using namespace QComicBook;

ImageTransformThread* ImageTransformThread::sm_thread = 0;

ImageTransformThread::ImageTransformThread(): QThread()
{
    _DEBUG;
    m_stopped = false;
}

ImageTransformThread::~ImageTransformThread()
{
    _DEBUG;
}

void ImageTransformThread::addJob(ImageTransformJob *job)
{
    _DEBUG;
    if (job)
    {
        m_jobmtx.lock();
        for (QList<ImageTransformJob *>::iterator it = m_jobs.begin(); it != m_jobs.end(); ++it)
        {
            ImageTransformJob *j = *it;
            if (j->key() == job->key())
            {
                _DEBUG << "removing duplicated job" << job->key();
                delete j;
                m_jobs.erase(it);
                break;
            }
        }
        m_jobs.append(job);
        _DEBUG << "num of jobs" << m_jobs.count();
        m_jobmtx.unlock();
        m_reqCond.wakeOne();
    }
}

void ImageTransformThread::run()
{
    _DEBUG;
    while (!m_stopped)
    {
        m_condMutex.lock();
        m_reqCond.wait(&m_condMutex);
        m_condMutex.unlock();

        for (;;)
        {
            m_jobmtx.lock();
            if (m_jobs.isEmpty())
            {
                m_jobmtx.unlock();
                break;
            }
            ImageTransformJob *job = m_jobs.first();
            _DEBUG << "got new job" << job->key();
            m_jobs.pop_front();
            m_jobmtx.unlock();

            job->execute();
            emit jobCompleted(ImageJobResult(job->key(), job->getResult()));
            delete job;
        }
    }
}

void ImageTransformThread::cancel()
{
    _DEBUG;
    m_jobmtx.lock();
    foreach (ImageTransformJob *job, m_jobs)
    {
        delete job;
    }
    m_jobs.clear();
    m_jobmtx.unlock();
}

void ImageTransformThread::stop()
{
    _DEBUG;
    m_stopped = true;
    cancel();
    m_reqCond.wakeOne();
}

ImageTransformThread* ImageTransformThread::get()
{
    if (!sm_thread)
    {
        sm_thread = new ImageTransformThread();
        sm_thread->start();
    }
    return sm_thread;
}
