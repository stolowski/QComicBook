#ifndef __IMGTRANSFORM_H
#define __IMGTRANSFORM_H

/*! \file imgtransform.h */

#include <cstdlib>

class QImage;

namespace QComicBook
{
		class ImgTransform
		{
			public:
				template <class T> class TransformBuffer
				{
					protected:
						T *buffer;
						T *end;
						int width, height;

					public:
						TransformBuffer(int w, int h): width(w), height(h)
						{
							buffer = new T [w*h];
							end = buffer + w*h;
						}
						TransformBuffer(): width(0), height(0), buffer(NULL)
						{
						}
						~TransformBuffer()
						{
							delete [] buffer;
						}
						inline void fill(T v)
						{
							for (T *p = buffer; p!=end; p++)
								*p = v;
						}
						inline T& operator()(int x, int y)
						{
							return buffer[x + y*width];		
						}
				};
					
			private:
				QImage *img;
				int w, h;
				int **acu;

				static float sin_lut[];
				static float cos_lut[];
				static bool lut_init;

			protected:
				void gaussianBlur();
				void enhanceEdges();
				void houghTransform();
			public:
				ImgTransform(QImage *image);
				~ImgTransform();

				void doTransform();

		};
}

#endif

