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

#ifndef __LOADERTHREADBASE_H
#define __LOADERTHREADBASE_H

#include <QThread>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

namespace QComicBook
{
    class ImgSink;
    class Page;
    
    struct LoadRequest
    {
        int pageNumber;
        bool twoPages;
        
        LoadRequest(int page, bool twoPages): pageNumber(page), twoPages(twoPages) {}
        bool operator==(const LoadRequest &r)
        {
            return pageNumber == r.pageNumber && twoPages == r.twoPages;
        }
    };
    
    class LoaderThreadBase: public QThread
    {
            Q_OBJECT

            protected:
                volatile QThread::Priority prio; //!<thread priority
                QList<LoadRequest> requests; //!<the list of requested pages
                ImgSink *sink;
                QMutex loaderMutex;
                QMutex condMutex;
                QMutex sinkMutex;
                QWaitCondition reqCond;
                volatile bool stopped;

                //! Main function of the thread.
                /*! Preloads requested pages from requests list using sink->getImage().
                 *  Stop if stopped flag is true.
                 *  @see ImgDirSink::getImage
                 */
                virtual void run();

                virtual bool process(const LoadRequest &req) = 0;

           public:
                LoaderThreadBase();
                virtual ~LoaderThreadBase();

                //! Changes priority of the loader thread.
                /*! @param p new priority
                 */
                virtual void setPriority(QThread::Priority p);

                //! Sets image source sink.
                /*! @param sink image sink used for retrieving (loading) images
                 */
                virtual void setSink(ImgSink *sink=NULL);
                
                //! Stops processing requests and exits thread execution.
                virtual void stop();
                
                
           public slots:
                //! Appends page to the list of pages to load.
                /*! @param page page to load
                 */
                virtual void request(int page);
                
                virtual void requestTwoPages(int page);
                
                //! Appends few pages to the list of pages to load.
                /*! @param first starting page
                 *  @param n number of pages to load in turn
                 */
                virtual void request(int first, int n);
                
                virtual void cancel(int page);
                virtual void cancelTwoPages(int page);
                virtual void cancelAll();
    };
}

#endif
