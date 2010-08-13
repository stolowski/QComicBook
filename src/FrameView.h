#ifndef __FRAME_VIEW_H
#define __FRAME_VIEW_H

#include "PageViewBase.h"
#include <QList>
#include <ComicFrame.h>

class QVBoxLayout;
class QLabel;

namespace QComicBook
{
	class FrameView: public PageViewBase
	{
		Q_OBJECT

		public:
			FrameView(QWidget *parent, int physicalPages, const ViewProperties& props);
			virtual ~FrameView();
			
		public slots:
			virtual void setImage(const Page &img1);
            virtual void setImage(const Page &img1, const Page &img2);
			virtual void setFrames(int page, const QList<ComicFrame> &frames);
			virtual void nextFrame();
			virtual void prevFrame();
            virtual void clear();
            virtual void gotoPage(int n);
            virtual void scrollToTop();
            virtual void scrollToBottom();
            virtual void propsChanged();
			virtual int visiblePages() const;
            virtual int viewWidth() const;
            virtual int currentPage() const;
			void gotoFrame(int n);
                        
		protected:
			int m_currentPage;
			int m_currentFrame;
			QList<ComicFrame> m_frames;
			QImage m_page;
			QVBoxLayout *m_layout;
			QLabel *m_img;
	};
}

#endif
