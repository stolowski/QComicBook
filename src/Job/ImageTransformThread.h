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

#ifndef __IMAGETRANSFORMTHREAD_H
#define __IMAGETRANSFORMTHREAD_H

#include <QThread>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

namespace QComicBook 
{
    class ImageTransformJob;
    class ImageJobResult;

    class ImageTransformThread: public QThread
    {
        Q_OBJECT

    public:
        ImageTransformThread();
        ~ImageTransformThread();

        static ImageTransformThread* get();

        void run();
        
    public slots:
        void stop();
        void cancel();
        void addJob(ImageTransformJob *job);
 
    signals:
        void jobCompleted(const ImageJobResult &);

    private:
        QMutex m_jobmtx;
        QMutex m_condMutex;
        QWaitCondition m_reqCond;
        bool m_stopped;
                
        QList<ImageTransformJob *> m_jobs;
        static ImageTransformThread *sm_thread;
    };
}

#endif
