#ifndef __DIRREADER_H
#define __DIRREADER_H

#include <QDir>
#include <QFileInfo>

class DirReader
{
	private:
		QDir::SortFlags flags;
		int maxDirDepth;
		int curDepth;

		void recurseDir(const QString &path);
		
	protected:
		virtual bool fileHandler(const QFileInfo &path) = 0;

	public:
		DirReader(QDir::SortFlags sortFlags, int maxDepth);
		virtual ~DirReader();

		void visit(const QString &path);
};

#endif

