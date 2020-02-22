#include "stars.h"
#include <qcolor.h>
#include <qimage.h>

struct star {
    int x;
    int y;
    QRgb color;
};

Stars::Stars(double freq, const QImage& img)
{
    const int width = img.width();
    const int height = img.height();

    n = (int)(width * height * freq);
    sky = new star[n];
    for (int i = 0; i < n; i++) {
        sky[i].x = gen(width);
        sky[i].y = gen(height);
        int base = 130 + gen(90);
        sky[i].color = qRgb(base + gen(36), base + gen(36), base + gen(36));
    }
}

Stars::~Stars()
{
    delete[] sky;
}

void Stars::render(QImage& img) const
{
    for (int i = 0; i < n; i++)
        img.setPixel(sky[i].x, sky[i].y, sky[i].color);
}
