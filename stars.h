#pragma once

#include "random.h"
struct star;
class QImage;

class Stars {
public:
    Stars(double, const QImage&);
    ~Stars();
    void render(QImage&) const;

private:
    int n;
    struct star* sky;
    Gen gen;
    // don't want to bother with copy
    Stars& operator=(const Stars&);
    Stars(const Stars&);
};
