#ifndef __IMLIBIMAGE_H
#define __IMLIBIMAGE_H

/*! \file imlibimage.h */

class QPaintDevice;
class QString;

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
			int load(const QString &path);
			void draw(QPaintDevice *p, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh);
			int width();
			int height();
			void rotate(int orient);
			ImlibImage* rotateClone(int orient);
			void reset();
	};
};

#endif

