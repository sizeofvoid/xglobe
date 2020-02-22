/*
 * random.cpp
 *
 * $Id: random.cpp,v 1.1 2000/06/15 09:43:29 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 2000 Marc Espie
 *
 */

#ifndef USE_RANDOM
#define USE_RAND
#endif

#include "random.h"
#include <cmath>
#include <cstdlib>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

bool Gen::initialized = false;
bool Gen::has_one = false;
double Gen::preserve;

Gen::Gen()
{
    if (!initialized) {
        int seed = time(NULL) + getpid();
#if defined(USE_RANDOM)
        srandom(seed);
#else
#if defined(USE_RAND)
        srand(seed);
#endif
#endif
        initialized = true;
    }
}

int Gen::draw() const
{
#if defined(USE_RANDOM)
    return random();
#else
#if defined(USE_RAND)
    return rand();
#endif
#endif
}

int Gen::operator()(int modulus) const
{
    int value = draw();
#ifdef ALLBITS_GOOD
    return value % modulus;
#else
    // so we've got a value up to RAND_MAX
    int divisor = RAND_MAX / modulus;
    // bad value, redo
    if (value >= divisor * modulus)
        return (*this)(modulus);
    else
        return value / divisor;
#endif
}

double Gen::gaussian()
{
    if (has_one) {
        has_one = false;
        return preserve;
    }
    double v1, v2, rsq;
    do {
        v1 = draw() / (double)(RAND_MAX / 2) - 1.0;
        v2 = draw() / (double)(RAND_MAX / 2) - 1.0;
        rsq = v1 * v1 + v2 * v2;
    } while (rsq >= 1.0 || rsq == 0.0);
    double fac = sqrt(-2.0 * log(rsq) / rsq);
    preserve = v1 * fac;
    has_one = true;
    return v2 * fac;
}
