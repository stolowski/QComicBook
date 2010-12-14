/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __FRAME_VIEW_H
#define __FRAME_VIEW_H

#include "PageViewBase.h"
#include "Page.h"
#include <ComicFrameList.h>

class QVBoxLayout;
class QLabel;

namespace QComicBook
{
	class FrameWidget;

	class FrameView: public PageViewBase
	{
		Q_OBJECT

		public:
			FrameView(QWidget *parent, int physicalPages, const ViewProperties& props);
			virtual ~FrameView();
			
			virtual void resizeEvent(QResizeEvent *e);

		public slots:
			virtual void setImage(const Page &img1);
            virtual void setImage(const Page &img1, const Page &img2);
			virtual void setFrames(const ComicFrameList &frames);
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
			FrameWidget *m_frame;
			Page m_page;
			ComicFrameList m_frames;
			QVBoxLayout *m_layout;
	};
}

#endif
