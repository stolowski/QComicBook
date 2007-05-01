#include "dirreader.h"

DirReader::DirReader(QDir::SortFlags sortFlags, int maxDepth): flags(sortFlags), maxDirDepth(maxDepth)
{
}

DirReader::~DirReader()
{
}

void DirReader::recurseDir(const QString &path)
{
        QDir dir(path);
        dir.setSorting(flags);
        dir.setFilter(QDir::AllDirs|QDir::Files);

        const QStringList files = dir.entryList();
	foreach (QString f, files)
        {
                if (f == "." || f == "..")
                        continue;

                QFileInfo finf(dir.absoluteFilePath(f));
		fileHandler(finf);
                if (finf.isDir() && (finf.absoluteFilePath() != path))
		{
			if (curDepth < maxDirDepth)
			{
				++curDepth;
				recurseDir(finf.absoluteFilePath());
			}
		}
        }
}

void DirReader::visit(const QString &path) 
{
	recurseDir(path);
}

