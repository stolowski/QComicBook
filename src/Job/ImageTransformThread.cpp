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
                _DEBUG << "removing duplicated job";
                delete j;
                m_jobs.erase(it);
                break;
            }
        }
        m_jobs.append(job);
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

        m_jobmtx.lock();
        if (!m_jobs.isEmpty())
        {
            _DEBUG << "got new job";

            ImageTransformJob *job = m_jobs.first();
            m_jobs.pop_front();
            m_jobmtx.unlock();

            job->execute();
            emit jobCompleted(ImageJobResult(job->key(), job->getResult()));
            delete job;
        }
        else
        {
            m_jobmtx.unlock();
        }
    }
}

void ImageTransformThread::stop()
{
    _DEBUG;
    m_stopped = true;
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
