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

#include "imgdirsink.h"
#include "imgcache.h"
#include "cbsettings.h"
#include <qimage.h>
#include <qstringlist.h>
#include <qprocess.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <utime.h>

ImgDirSink::ImgDirSink(int cachesize): ImgSink(), cachemtx(true)/*{{{*/
{
	thloader.setSink(this);
	cache = new ImgCache(cachesize);
}/*}}}*/

ImgDirSink::ImgDirSink(const QString &path, int cachesize): cachemtx(true), dirpath(QString::null)/*{{{*/
{
	thloader.setSink(this);
	cache = new ImgCache(cachesize);
	open(path);
}/*}}}*/

ImgDirSink::~ImgDirSink()/*{{{*/
{
	thloader.stop();
	wait(); //wait for preload thread
	thloader.wait(); //wait for thumbnail loader thread
	close();
	delete cache;
}/*}}}*/

void ImgDirSink::setComicBookName(const QString &name)/*{{{*/
{
	cbname = name;
}/*}}}*/

QString ImgDirSink::memPrefix(int &s) const/*{{{*/
{
	QString mempfix;
	if (s < 1024)
	{
		mempfix = tr("bytes");
	}
	else if (s < 1024*1024)
	{
		s /= 1024;
		mempfix = tr("Kbytes");
	}
	else
	{
		s /= 1024*1024;
		mempfix = tr("Mbytes");
	}
	return mempfix;
}/*}}}*/

void ImgDirSink::recurseDir(const QString &s)/*{{{*/
{
	QDir dir(s);
	dir.setSorting(QDir::DirsFirst|QDir::Name);
	dir.setMatchAllDirs(true);
	const QStringList files = dir.entryList();
	for (QStringList::const_iterator it = files.begin(); it!=files.end(); ++it)
	{
		if (*it == "." || *it == "..")
			continue;

		QFileInfo finf(dir.absFilePath(*it, false));
		if (finf.isFile())
		{
			if ((*it).endsWith(".jpg", false) || (*it).endsWith(".jpeg", false) || (*it).endsWith(".png", false) || (*it).endsWith(".gif", false) || (*it).endsWith(".xpm", false))
				imgfiles.append(finf.absFilePath());
			else if ((*it).endsWith(".nfo", false) || (*it) == "file_id.diz")
				txtfiles.append(finf.absFilePath());
			else
				otherfiles.append(finf.absFilePath());
		}
		else if (finf.isDir() && (finf.absFilePath()!=s))
		{
			dirs.append(finf.absFilePath());
			recurseDir(finf.absFilePath());
		}
	}
}/*}}}*/

int ImgDirSink::open(const QString &path)/*{{{*/
{
	int status;
	QFileInfo info(path);
	if (!info.exists())
		status = SINKERR_NOTFOUND;
	else
	{
		if (info.isDir())
		{
			if (info.isReadable() && info.isExecutable())
			{
				dirpath = path;
				recurseDir(path);
				status = (numOfImages() > 0) ? 0 : SINKERR_EMPTY;
			}
			else
				status = SINKERR_ACCESS;
		}
		else status = SINKERR_NOTDIR;
	}
	setComicBookName(path);
	if (status == 0)
		emit sinkReady(path);
	else
		emit sinkError(status);
	return status;
}/*}}}*/

void ImgDirSink::close()/*{{{*/
{
	dirpath = QString::null;
	imgfiles.clear();
	txtfiles.clear();
	otherfiles.clear();
	dirs.clear();
}/*}}}*/

QString ImgDirSink::getName(int maxlen)/*{{{*/
{
	if (dirpath.length() < maxlen)
		return dirpath;
	QString tmpname = "..." + dirpath.right(maxlen-3);
	return tmpname;
}/*}}}*/

QString ImgDirSink::getFullName()/*{{{*/
{
	return dirpath;
}/*}}}*/

QStringList ImgDirSink::getDescription() const/*{{{*/
{
	QStringList desc;
	
	for (QStringList::const_iterator it = txtfiles.begin(); it!=txtfiles.end(); it++)
	{
		QFileInfo finfo(*it);
		QFile f(*it);
		if (f.open(IO_ReadOnly) && (f.size() < MAX_TEXTFILE_SIZE))
		{
			QString cont;
			QTextStream str(&f);
			while (!str.atEnd())
				cont += str.readLine() + "\n";
			f.close();
			desc.append(finfo.fileName()); //append file name
			desc.append(cont); //and contents
		}
	}
	return desc;
}/*}}}*/

QString ImgDirSink::getStats() const/*{{{*/
{
	int w, h, p;
	int cmem = 0;
	
	cachemtx.lock();

	const int cachecnt = cache->count();

	//
	// calculate occupied memory and dimensions of the largest image
	p = w = h = 0;
	for (QCacheIterator<QImage> it(*cache); it.current(); ++it)
	{
		QImage *img = it.current();
		int tmp = img->width() * img->height();
		if (tmp > p)
		{
			p = tmp;
			w = img->width();
			h = img->height();
		}
		cmem += it.current()->numBytes();
	}

	cachemtx.unlock();

	int amem = (cachecnt!=0) ? cmem / cachecnt : 0;
	const QString cmempfix = memPrefix(cmem);
	const QString amempfix = memPrefix(amem);
	QString info;

	QTextStream str(&info, IO_WriteOnly);
	str << tr("Number of pages") << ": " << numOfImages() << endl << 
	       tr("Cached pages") << ": " << cachecnt << endl <<
	       tr("Cached pages memory") << ": " << cmem << " " << cmempfix << endl <<
	       tr("Average page memory") << ": " << amem << " " << amempfix << endl <<
	       tr("Largest cached image") << ": " << w << "x" << h << endl;
	return info;
}/*}}}*/

QImage ImgDirSink::getImage(unsigned int num, int &result, int preload)/*{{{*/
{
	QImage rimg;
	
	result = SINKERR_LOADERROR;

	if (num < imgfiles.count())
	{
		QImage *img = NULL; //cached image pointer

		cachemtx.lock();

		//
		// try to find in cache first
		if (img = cache->find(imgfiles[num]))
		{
			result = 0;
			rimg = *img;
		}
		else		
		{
			if (rimg.load(imgfiles[num]))
			{
				result = 0;
				if (cache->maxCost() > 0)
					cache->insert(imgfiles[num], &rimg);
			}
		}
		cachemtx.unlock();
	}

	cachemtx.lock();
	//
	// optionally start preload thread; don't preload for cache size =< 2
	if (preload>0 && cache->maxCost()> 2 && result == 0)
	{
		pre = num + 1; //page to preload
		if (pre < imgfiles.count()) // && (!cache->find(imgfiles[pre])))
		{
			precnt = preload;
			cachemtx.unlock();
			start();
		}
		else
			cachemtx.unlock();
	}
	else
		cachemtx.unlock();
	
	return rimg;
}/*}}}*/

QImage ImgDirSink::getThumbnail(unsigned int num, int w, int h, int &result, bool thumbcache)/*{{{*/
{
	QImage rimg;
	QString thname;

	//
	// try to load cached thumbnail
	if (thumbcache)
	{
		thname = ComicBookSettings::thumbnailsDir() + "/" + cbname.remove('/') + QString::number(num) + ".jpg";
		if (rimg.load(thname) && rimg.width()<=w && rimg.height()>=h)
		{
			result = 1;
			//
			// "touch" the file
			utime(thname.local8Bit(), NULL);
			return rimg;
		}
	}
	
	result = 0;
	cachemtx.lock();
	if (QImage *img = cache->find(imgfiles[num]))
	{
		rimg = *img;
		cachemtx.unlock();
		result = 1;
	}
	else
	{
		cachemtx.unlock();
		result = rimg.load(imgfiles[num]);
	}
	const QImage tmp(rimg.smoothScale(w, h, QImage::ScaleMin));
	if (thumbcache)
		tmp.save(thname, "JPEG", 75);
	return tmp;
}/*}}}*/

void ImgDirSink::setThumbnailReciever(QObject *rcv)/*{{{*/
{
	thloader.setReciever(rcv);
}/*}}}*/

void ImgDirSink::requestThumbnail(int num)/*{{{*/
{
	thloader.requestThumbnail(num);
}/*}}}*/

void ImgDirSink::requestThumbnails(int first, int n)/*{{{*/
{
	thloader.requestThumbnails(first, n);
}/*}}}*/

int ImgDirSink::numOfImages() const/*{{{*/
{
	return imgfiles.count();
}/*}}}*/

QStringList ImgDirSink::getAllfiles() const/*{{{*/
{
	return imgfiles + txtfiles + otherfiles;
}/*}}}*/

QStringList ImgDirSink::getAlldirs() const/*{{{*/
{
	return dirs;
}/*}}}*/

QStringList ImgDirSink::getAllimgfiles() const/*{{{*/
{
	return imgfiles;
}/*}}}*/

void ImgDirSink::run()/*{{{*/
{
	cachemtx.lock();
	for (int i=0; i<precnt; i++)
	{
		if (pre + i >= imgfiles.count())
			break;
		if (!cache->find(imgfiles[pre+i]))
		{
			QImage img;
			//std::cout << "preloading: " << imgfiles[pre+i] << std::endl;
			if (img.load(imgfiles[pre+i]))
				cache->insert(imgfiles[pre+i], &img);
		}
	}
	cachemtx.unlock();
}/*}}}*/

void ImgDirSink::removeThumbnails(int days)/*{{{*/
{	
	if (days < 1)
		return;

	const QDateTime currdate = QDateTime::currentDateTime();
	
	QDir dir(ComicBookSettings::thumbnailsDir(), "*.jpg", QDir::Unsorted, QDir::Files|QDir::NoSymLinks);
	const QStringList files = dir.entryList();
	for (QStringList::const_iterator it = files.begin(); it!=files.end(); it++)
	{
		QFileInfo finfo(dir.absFilePath(*it));
		if (finfo.lastModified().daysTo(currdate) > days)
			dir.remove(*it);
	}
}/*}}}*/

ThumbnailLoader& ImgDirSink::thumbnailLoader()/*{{{*/
{
	return thloader;
}/*}}}*/

