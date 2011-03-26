#ifndef __DIRTREE_H
#define __DIRTREE_H

#include <QString>
#include <QList>

namespace QComicBook
{
    class FileEntry
    {
	public:
	    FileEntry(const QString &name);
	    virtual ~FileEntry();

	    virtual void dump();
	protected:
	    QString name;
    };

    class DirEntry: FileEntry
    {
	public:
	    DirEntry();
	    DirEntry(const QString &name);
	    ~DirEntry();

	    DirEntry* addDir(const QString &name);
	    FileEntry* addFile(const QString &name);

	    virtual void dump();
	private:
	    QList<FileEntry*> entries;
    };

    /*class DirTree
    {
	DirTree();
	private:
    };*/
}

#endif
