#ifndef __COMICBOOKDEBUG_H
#define __COMICBOOKDEBUG_H

#include <QDebug>

#define _DEBUG qDebug() << Q_FUNC_INFO << ':'

#endif
