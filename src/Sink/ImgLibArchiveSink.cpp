#include "ImgLibArchiveSink.h"
#include "Page.h"
#include "Thumbnail.h"
#include <QStringList>
#include <QImage>
#include <QMap>
#include <QFile>
#include <iostream>
#include <cassert>
#include "DirTree.h"
#include <archive.h>
#include <archive_entry.h>

using namespace QComicBook;

ImgLibArchiveSink::ImgLibArchiveSink(int cacheSize): ImgSink(cacheSize), root(0)
{
}

ImgLibArchiveSink::~ImgLibArchiveSink()
{
}

QStringList ImgLibArchiveSink::splitPath(const QString &path)
{
	QStringList parts = path.split("/"); //FIXME
	return parts;
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
		return SINKERR_OTHER;
	}

	archive_entry *e;
	root = new DirEntry();
	QMap<QString, DirEntry*> fullPathMap;

	while (archive_read_next_header(a, &e) == ARCHIVE_OK) {
		std::cout << archive_entry_pathname(e) << std::endl;
		QString fullpath(archive_entry_pathname(e));
		if (fullpath.endsWith("/")) // FIXME
		{
			fullpath = fullpath.left(fullpath.length()-1);
		}
		QStringList parts = splitPath(fullpath);
		const QString name(parts.last());
		DirEntry *parent = NULL;
		if (parts.size() >= 2)
		{
			parts.pop_back();
			const QString tmpfullpath = parts.join("/");
			assert(fullPathMap.contains(tmpfullpath) == true);
			parent = fullPathMap[tmpfullpath];
		}
		else
		{
			parent = root;
		}

		if (archive_entry_filetype(e) == AE_IFDIR)
		{
			DirEntry *d = parent->addDir(name);
			fullPathMap.insert(fullpath, d);
		}
		else if (archive_entry_filetype(e) == AE_IFREG)
		{
			parent->addFile(name);
		}
		archive_read_data_skip(a);
	}

	r = archive_read_finish(a);

	setComicBookName("TEST", path);
	archivepath = path;

	return 0;
}

void ImgLibArchiveSink::close()
{
}

QImage ImgLibArchiveSink::image(unsigned int num, int &result)
{
  int r;
  size_t size;
  struct archive_entry *ae;

  //char buff[1024*1024];
  //int buffsize = 1024*1024;

  struct archive *a = archive_read_new();
  struct archive *ext;
  archive_read_support_format_all(a);
  archive_read_support_compression_all(a);
  ext = archive_write_disk_new();
  archive_write_disk_set_standard_lookup(ext);

  r = archive_read_open_filename(a, archivepath.toLocal8Bit(), 16384);
  if (r != ARCHIVE_OK) {
    /* ERROR */
  }
  r = archive_read_next_header(a, &ae);
  if (r != ARCHIVE_OK) {
    /* ERROR */
  }

  r = archive_write_header(ext, ae);

  const void *buff;
  off_t offset;

  for (;;) {
    r = archive_read_data_block(a, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
		break;
      
    //if (r != ARCHIVE_OK)
    //  return (r);
    r = archive_write_data_block(ext, buff, size, offset);
    //if (r != ARCHIVE_OK) {
    //  fprintf(stderr, "%s\n", archive_error_string(aw));
    //  return (r);
  }

  /*QFile outfile("/tmp/obraz");
  outfile.open(QIODevice::WriteOnly);
  for (;;) {
    size = archive_read_data(a, buff, buffsize);
    if (size < 0) {
      ERROR 
    }
    if (size == 0)
      break;
    outfile.write(buff, size);
  }
  outfile.close();*/

  std::cout << "obraz\n";

  archive_read_finish(a);

  return QImage();
}

Thumbnail ImgLibArchiveSink::getThumbnail(unsigned int num, bool thumbcache)
{
}

int ImgLibArchiveSink::numOfImages() const
{
	return 1;
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

bool ImgLibArchiveSink::timestampDiffers(int page) const
{
	return false;
}

bool ImgLibArchiveSink::hasModifiedFiles() const
{
	return false;
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

