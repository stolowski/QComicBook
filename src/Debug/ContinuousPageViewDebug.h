/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2012 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

#ifndef __CONTINUOUS_PAGE_VIEW_DEBUG_H
#define __CONTINUOUS_PAGE_VIEW_DEBUG_H

#include <QWidget>
#include <QPointer>
#include "../ui_ContinuousPageViewDebug.h"

class QGraphicsScene;

namespace QComicBook
{
    class ContinuousPageView;

    class ContinuousPageViewDebug: public QWidget, private Ui::ContinuousPageViewDebug
    {
        Q_OBJECT

        public:
            ContinuousPageViewDebug(QWidget *parent=0);
            ~ContinuousPageViewDebug();

        public Q_SLOTS:
            void setView(ContinuousPageView *view);

        private Q_SLOTS:
            void refreshClicked();
            void setZoomFactor(double v);

        private:
            void refresh();
            void dumpTextData();
            void drawRangeLines();
            void drawPageRectangles();

            QPointer<ContinuousPageView> m_view;
            QGraphicsScene *m_scene;
    };
}

#endif
