#include "DirTree.h"
#include <iostream>

using namespace QComicBook;

FileEntry::FileEntry(const QString &name): name(name)
{
}

FileEntry::~FileEntry()
{
}
	    
void FileEntry::dump()
{
    std::cout << name.toStdString() << std::endl;
}
	    
DirEntry::DirEntry(): FileEntry(QString::null)
{
}

DirEntry::DirEntry(const QString &name): FileEntry(name)
{
}

DirEntry::~DirEntry()
{
    for (QList<FileEntry *>::iterator it = entries.begin(); it != entries.end(); ++it)
    {
	delete *it;
    }
}

DirEntry* DirEntry::addDir(const QString &name)
{
    DirEntry *d = new DirEntry(name);
    entries.append(d);
    return d;
}

FileEntry* DirEntry::addFile(const QString &name)
{
    FileEntry *f = new FileEntry(name);
    entries.append(f);
    return f;
}

void DirEntry::dump()
{
    std::cout << name.toStdString() << ": " << entries.size() << std::endl;
    for (QList<FileEntry *>::const_iterator it = entries.begin(); it != entries.end(); it++)
    {
	(*it)->dump();
    }
}
