/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2013 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#include "ImgLibArchiveSink.h"
#include "Page.h"
#include "Thumbnail.h"
#include <QStringList>
#include <QImage>
#include <QList>
#include <QDebug>
#include <cassert>
#include "FileEntry.h"
#include <archive.h>
#include <archive_entry.h>

using namespace QComicBook;

ImgLibArchiveSink::ImgLibArchiveSink(int cacheSize): ImgSink(cacheSize)
{
}

ImgLibArchiveSink::~ImgLibArchiveSink()
{
}

void ImgLibArchiveSink::setCacheSize(int cacheSize, bool autoAdjust)
{
}

int ImgLibArchiveSink::open(const QString &path)
{
    archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    
    int r = archive_read_open_filename(a, path.toLocal8Bit(), 10240);
    if (r != ARCHIVE_OK)
    {
        qDebug() << "ERROR";
        return SINKERR_OTHER;
    }

    archive_entry *e;
    
    while (archive_read_next_header(a, &e) == ARCHIVE_OK)
    {
        QString fullpath(archive_entry_pathname(e));
        qDebug() << "libarchive full:" << fullpath;
        if (fullpath.endsWith("/")) // FIXME
	{
            fullpath = fullpath.left(fullpath.length()-1);
        }

        if (archive_entry_filetype(e) == AE_IFREG)
	{
            qDebug() << "libarchive file:" << fullpath;
            FileEntry fe(fullpath);
            imgfiles.append(fe);
        }
    }

    r = archive_read_free(a);
    
    setComicBookName("TEST", path);
    archivepath = path;
    
    return 0;
}

void ImgLibArchiveSink::sort(const PageSorter &sorter)
{
}

void ImgLibArchiveSink::close()
{
}

QImage ImgLibArchiveSink::image(unsigned int num, int &result)
{
  const QString requested_path = imgfiles[num].getFullFileName();

  archive *a = archive_read_new();

  archive_read_support_format_all(a);
  archive_read_support_compression_all(a);
 
  int r = archive_read_open_filename(a, archivepath.toLocal8Bit(), 16384);
  if (r != ARCHIVE_OK) {
      result = SINKERR_LOADERROR;
  }

  QString tmpfilepath;

  size_t size;
  struct archive_entry *ae;

  archive *ext = archive_write_disk_new();
  archive_write_disk_set_standard_lookup(ext);
  archive_write_disk_set_options(ext, 
                                 ARCHIVE_EXTRACT_SECURE_SYMLINKS
                                 |ARCHIVE_EXTRACT_SECURE_NODOTDOT
                                 |ARCHIVE_EXTRACT_NO_OVERWRITE);


  while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
  {
      QString fullpath(archive_entry_pathname(ae));
      if (archive_entry_filetype(ae) == AE_IFREG && fullpath == requested_path)
      {
          r = archive_write_header(ext, ae);
                    
          for (;;)
          {
              const void *buff;
              off_t offset;

              r = archive_read_data_block(a, &buff, &size, &offset);
              if (r == ARCHIVE_EOF)
                  break;
              
              if (r != ARCHIVE_OK)
              {
                  qWarning() << archive_error_string(a);
                  result = SINKERR_LOADERROR;
                  break;
              }
          
              r = archive_write_data_block(ext, buff, size, offset);
              
              if (r != ARCHIVE_OK)
              {
                  qWarning() << archive_error_string(ext);
                  result = SINKERR_LOADERROR;
                  break;
              }
          }
          archive_write_close(ext);
          archive_write_free(ext);

          tmpfilepath = imgfiles[num].getFileName();
          break;
      }
  }

  if (!tmpfilepath.isEmpty())
      return QImage(tmpfilepath);

  archive_read_finish(a);

  return QImage();
}

Thumbnail ImgLibArchiveSink::getThumbnail(unsigned int num, bool thumbcache)
{
    return Thumbnail();
}

int ImgLibArchiveSink::numOfImages() const
{
    return imgfiles.size();
}

void ImgLibArchiveSink::setComicBookName(const QString &name, const QString &fullName)
{
}

QString ImgLibArchiveSink::getName(int maxlen) const
{
    return "TEST";
}

QString ImgLibArchiveSink::getFullName() const
{
    return "TEST";
}

QString ImgLibArchiveSink::getFullFileName(int page) const
{
    return "TEST";
}

QStringList ImgLibArchiveSink::getDescription() const
{
    return QStringList();
}

bool ImgLibArchiveSink::supportsNext() const
{
    return false;
}

QString ImgLibArchiveSink::getNext() const
{
    return QString::null;
}

QString ImgLibArchiveSink::getPrevious() const
{
    return QString::null;
}
