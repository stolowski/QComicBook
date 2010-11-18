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

#ifndef __COLOR_PUSH_BUTTON_H
#define __COLOR_PUSH_BUTTON_H

#include <QPushButton>

//! Push button implementation that displays a filled rectangle.
class ColorPushButton: public QPushButton
{
	public:
		ColorPushButton(QWidget *parent = 0);
		ColorPushButton(const QString &text, QWidget *parent = 0);

		QColor getColor() const;
	
	public slots:
		void setColor(const QColor &c);

	protected:
		virtual void paintEvent(QPaintEvent *e);

	private:
		QColor color;
};

#endif
