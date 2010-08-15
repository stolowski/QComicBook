#ifndef __COMIC_FRAME_LIST_H
#define __COMIC_FRAME_LIST_H

#include <QVector>
#include <ComicFrame.h>

namespace QComicBook
{
	class ComicFrameList
	{
		public:
			ComicFrameList(int page=-1);
			virtual ~ComicFrameList();

			void append(const ComicFrame &f);
			void sort();
			const ComicFrame& operator[](int idx);
			int count() const { return m_frames.count(); }
			int pageNumber() const { return m_page; }

		private:
			int m_page;
			QVector<ComicFrame> m_frames;
	};
}

#endif
