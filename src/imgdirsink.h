/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski <yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __IMGDIRSINK_H
#define __IMGDIRSINK_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qmutex.h>
#include <qthread.h>
#include "thumbnailloader.h"
#include "imgsink.h"

//
// maximum size of description file (won't load files larger than that)
#define MAX_TEXTFILE_SIZE 65535

class QImage;
class ImgCache;

enum
{
	SINKERR_ACCESS = 1, //can't access file or directory
	SINKERR_UNKNOWNFILE, //unknown archive format
	SINKERR_LOADERROR,  //can't load image file
	SINKERR_NOTFOUND,
	SINKERR_NOTFILE,   //not a regular file
	SINKERR_NOTDIR,    //not a directory
	SINKERR_EMPTY,     //no images inside
	SINKERR_OTHER
};

class ImgDirSink: public ImgSink, protected QThread
{
	Q_OBJECT
		
	private:
		ThumbnailLoader thloader;
		
	protected:
		mutable QMutex cachemtx; //mutex for cache, pre, precnt
		ImgCache *cache;
		int pre; //number of first page to preload
		int precnt; //number of pages to preload starting from pre
		
		QString dirpath; //path to directory
		QString cbname; //comic book name (directory path by default)
		QStringList imgfiles; //list of images files in directory
		QStringList txtfiles; //text files (.nfo, file_id.diz)
		QStringList otherfiles; //list of other files
		QStringList dirs; //directories

		void recurseDir(const QString &s);
		QString memPrefix(int &s) const;

		virtual void run();

		void setComicBookName(const QString &name);
		
	public slots:
		virtual void setThumbnailReciever(QObject *rcv);
		virtual void requestThumbnail(int num);
		virtual void requestThumbnails(int first, int n);

	public:
		ImgDirSink(int cachesize=1);
		ImgDirSink(const QString &path, int cachesize=1);
		virtual ~ImgDirSink();

		virtual int open(const QString &path);
		virtual void close();
		virtual QImage getImage(unsigned int num, int &result, int preload=0);
		virtual QImage getThumbnail(unsigned int num, int w, int h, int &result, bool thumbcache=true);
		virtual int numOfImages() const;
		virtual QString getName(int maxlen = 50);
		virtual QString getFullName();    
		virtual QStringList getDescription() const;
		virtual QString getStats() const;

		virtual QStringList getAllfiles() const;
		virtual QStringList getAlldirs() const;
		virtual QStringList getAllimgfiles() const;

		virtual ThumbnailLoader& thumbnailLoader();

		//
		// removes old thumbnails
		static void removeThumbnails(int days);
};

#endif

