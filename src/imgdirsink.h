/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005 Pawel Stolowski < yogin@linux.bydg.org>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

/*! \file imgdirsink.h */

#ifndef __IMGDIRSINK_H
#define __IMGDIRSINK_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qmutex.h>
#include "thumbnailloader.h"
#include "imgloader.h"
#include "imgsink.h"

//
// maximum size of description file (won't load files larger than that)
#define MAX_TEXTFILE_SIZE 65535

class QImage;
class ImgCache;

//! Possible errors.
enum SinkError
{
	SINKERR_ACCESS = 1, //!<can't access file or directory
	SINKERR_UNKNOWNFILE, //!<unknown archive format
	SINKERR_NOTSUPPORTED, //!<known archive type, but not supported
	SINKERR_LOADERROR,  //!<can't load image file
	SINKERR_NOTFOUND, //!<file not found
	SINKERR_NOTFILE,   //!<not a regular file
	SINKERR_NOTDIR,    //!<not a directory
	SINKERR_EMPTY,     //!<no images inside
	SINKERR_ARCHEXIT, //!<archiver exited with error
	SINKERR_OTHER  //!<another kind of error
};

//! Comic book directory sink.
/*! Allows opening directories containing image files. */
class ImgDirSink: public ImgSink
{
	Q_OBJECT
		
	private:
		ThumbnailLoaderThread thloader;
		ImgLoaderThread imgloader;
		
	protected:
		mutable QMutex cachemtx; //!< mutex for cache, pre, precnt
		mutable QMutex listmtx; //!< mutex for imgfiles
		ImgCache *cache;
		QString dirpath; //!< path to directory
		QString cbname; //!< comic book name (directory path by default)
		QStringList imgfiles; //!< list of images files in directory
		QStringList txtfiles; //!< text files (.nfo, file_id.diz)
		QStringList otherfiles; //!< list of other files
		QStringList dirs; //!< directories

		//! Scans directories recursively for image and text files.
		/*! Scans directories for jpg, png, gif, xpm image files and .nfo and file_id.diz text files.
		 *  Images file names are stored in imgfiles; text file names are stored in txtfiles.
		 *  Other files are store in otherfiles. All directories are stored in dirs.
		 *  @param s starting directory 
		 *  @see imgfiles
		 *  @see txtfiles
		 *  @see otherfiles
		 *  @see dirs */
		void recurseDir(const QString &s);
		QString memPrefix(int &s) const;
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
		virtual Thumbnail* getThumbnail(int num, bool thumbcache=true);
		virtual int numOfImages() const;
		virtual QString getName(int maxlen = 50);
		virtual QString getFullName();    
		virtual QStringList getDescription() const;
		virtual QString getStats() const;

		virtual QStringList getAllfiles() const;
		virtual QStringList getAlldirs() const;
		virtual QStringList getAllimgfiles() const;

		//! Returns the instance of thumbnail loader.
		/*! @return thumbnail loader instance */
		virtual ThumbnailLoaderThread& thumbnailLoader();

		//
		//! Removes old thumbnails.
		/*! @param days thumbnails older than this number will be removed */
		static void removeThumbnails(int days);
};

#endif

