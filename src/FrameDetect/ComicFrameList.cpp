#include <ComicFrameList.h>
#include <QDebug>

using namespace QComicBook;

ComicFrameList::ComicFrameList(int page)
	: m_page(page)
{

}

ComicFrameList::~ComicFrameList()
{

}

void ComicFrameList::append(const ComicFrame &f)
{
	m_frames.append(f);
}

void ComicFrameList::sort()
{
	qDebug() << "frames sort";
}

const ComicFrame& ComicFrameList::operator[](int idx)
{
	return m_frames[idx];
}

