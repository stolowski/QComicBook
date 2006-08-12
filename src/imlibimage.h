#ifndef __IMLIBIMAGE_H
#define __IMLIBIMAGE_H

/*! \file imlibimage.h */

class QPaintDevice;

namespace QComicBook
{
	class ImlibImage
	{
		private:
			void *data;
			void *context;
			int w, h;

		public:
			ImlibImage();
			~ImlibImage();
			int load(const char *path);
			void draw(QPaintDevice *p, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, int orient=0);
			int width();
			int height();
			void rotate(int orient);
			ImlibImage* rotateClone(int orient);
			void reset();
	};
};

#endif

