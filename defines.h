/*
 * defines.h
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 2001 Marc Espie
 */

#include <assert.h>

#if QT_VERSION >= 200
#define QTBLACK Qt::black
#define QTWHITE Qt::white
#define QTBLUE Qt::blue
#define QTRED Qt::red
#else
#define QTBLACK black
#define QTWHITE white
#define QTBLUE blue
#define QTRED red
#endif

static inline QRgb* scan32(QImage& img, int x, int y)
{
    assert(img.depth() == 32);
    return reinterpret_cast<QRgb*>(img.scanLine(y)) + x;
}
