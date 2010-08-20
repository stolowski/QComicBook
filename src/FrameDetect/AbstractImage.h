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

#ifndef ABSTRACTIMAGE_H
#define ABSTRACTIMAGE_H

namespace QComicBook
{
	template <typename T>
		class AbstractImage
		{
			public:
				typedef T ImageDataType;

				AbstractImage(int w, int h)
					: m_width(w)
					  , m_height(h)
				{
					data = new T [w*h];
				}

				virtual ~AbstractImage()
				{
					delete [] data;
				}

				T& operator()(int x, int y)
				{
					return data[x + y*m_width];
				}

				T& at(int x, int y)
				{
					return data[x + y*m_width];
				}

				const T& at(int x, int y) const
				{
					return data[x + y*m_width];
				}

				const T* scanline(int y) const
				{
					return data + y*m_width;
				}

				void fill(const T &val)
				{
					for (int i=0; i<m_width*m_height; i++)
					{
						data[i] = val;
					}
				}

				int width() const
				{
					return m_width;
				}

				int height() const
				{
					return m_height;
				}

			protected:
				T *data;
				int m_width;
				int m_height;
		};
}

#endif // ABSTRACTIMAGE_H
