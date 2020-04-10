/*
 * moonpos.cpp
 *
 * $Id: moonpos.cpp,v 1.2 2000/06/15 09:43:29 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 * Copyright (C) 2020 Rafael Sadowski
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: moonpos.cpp,v $
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.2  1999/09/19 19:44:17  thorsten
 * moonpos bugfix (Andrew)
 *
 * Revision 0.1  1999/07/19 12:42:13  thorsten
 * initial revision
 *
 *
 */

/******* from xplanet sources: */
/******************************************************************************
    xplanet-0.20 - render an image of the earth into an X window
    Copyright (C) 1999 Hari Nair <hari@alumni.caltech.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

******************************************************************************/

/* Based on Chapter 30 of Astronomical Formulae for Calculators by Meeus */

#include "moonpos.h"
#include <cmath>

/* ------------------------------------------------------------------------*/

double MoonPos::julian(int year, int month, int day, int hour, int min, int sec)
{
    int a, b, c, d;
    double e, jd;

    if (month < 3) {
        year--;
        month += 12;
    }

    a = year / 100;
    b = 2 - a + a / 4;
    c = int(365.25 * year);
    d = int(30.6001 * (month + 1));
    e = day + hour / 24.0 + min / 1400.0 + sec / 86400.0;

    jd = b + c + d + e + 1720994.5;

    return jd;
}

/* ------------------------------------------------------------------------*/

double MoonPos::jcentury(time_t thetime)
{
    struct tm* tm;
    double jd;

    tm = gmtime(&thetime);
    jd = julian(tm->tm_year + 1900,
        tm->tm_mon + 1,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec);

    return ((jd - 2415020) / 36525);
}

/* ------------------------------------------------------------------------*/

double MoonPos::poly(double a1, double a2, double a3, double a4, double t)
{
    return (a1 + t * (a2 + t * (a3 + t * a4)));
}

/* ------------------------------------------------------------------------*/

double MoonPos::compute_obliquity(double T)
{
    double d = poly(23.452294, -1.30125E-2, -1.64E-6, 5.03E-7, T) * (M_PI / 180);
    return d;
}

/* ------------------------------------------------------------------------*/

void MoonPos::compute_ra_dec(double lon, double lat, double& alpha, double& delta, double eps)
{
    delta = asin(sin(eps) * sin(lon) * cos(lat) + sin(lat) * cos(eps));
    alpha = (atan2(cos(eps) * sin(lon) - tan(lat) * sin(eps), cos(lon)) / (M_PI / 180));
    alpha /= 15;
    alpha = fmod(alpha, 24.0);
    if (alpha < 0)
        alpha += 24;
}

/* ------------------------------------------------------------------------*/

double MoonPos::S(double T)
{
    return sin(T * (M_PI / 180));
}

/* ------------------------------------------------------------------------*/

double MoonPos::C(double T)
{
    return cos(T * (M_PI / 180));
}

/* ------------------------------------------------------------------------*/

double MoonPos::gmst(double T, time_t thetime)
{
    double g = poly(6.6460656, 2400.051262, 0.00002581, 0, T);

    double currgmt = (gmtime(&thetime)->tm_hour
        + gmtime(&thetime)->tm_min / 60.
        + gmtime(&thetime)->tm_sec / 3600.);
    currgmt *= 1.002737908;
    g += currgmt;
    g = fmod(g, 24.0);
    if (g < 0)
        g += 24;
    return (g);
}

/* ------------------------------------------------------------------------*/

void MoonPos::moonpos(double& moonlon, double& moonlat, double& moondist,
    double T)
{
    double M = poly(358.475833, 35999.04975, -1.500E-4, -3.30E-6, T);
    double Lp = poly(270.434164, 481267.8831, -1.133E-3, 1.90E-6, T);
    double Mp = poly(296.104608, 477198.8491, 9.192E-3, 1.44E-5, T);
    double D = poly(350.737486, 445267.1142, -1.436E-3, 1.90E-6, T);
    double F = poly(11.250889, 483202.0251, -3.211E-3, -3.00E-7, T);
    double omega = poly(259.183275, -1934.142, 2.078E-3, 2.2E-6, T);
    Lp = Lp + ((2.330E-4) * S(51.2 + 20.2 * T) + (1.964E-3) * S(omega) + (3.964E-3) * S(poly(346.56, 132.87, -9.1731E-3, 0, T)));
    M = M - (1.778E-3) * S(51.2 + 20.2 * T);
    Mp = Mp + ((8.170E-4) * S(51.2 + 20.2 * T) + (2.541E-3) * S(omega) + (3.964E-3) * S(poly(346.56, 132.87, -9.1731E-3, 0, T)));
    D = D + ((2.011E-3) * S(51.2 + 20.2 * T) + (1.964E-3) * S(omega) + (3.964E-3) * S(poly(346.56, 132.87, -9.1731E-3, 0, T)));
    F = F - ((2.4691E-2) * S(omega) - (4.3280E-3) * S(omega + 275.05 - 2.3 * T) + (3.9640E-3) * S(poly(346.56, 132.87, -9.1731E-3, 0, T)));
    double E = 1 - T * (2.495E-3 + 7.52E-6 * T);
    double E2 = E * E;
    double lambda = (Lp
        + 6.28875 * S(Mp)
        + 1.274018 * S(2 * D - Mp) + .658309 * S(2 * D)
        + .213616 * S(2 * Mp) - E * .185596 * S(M)
        - .114336 * S(2 * F) + .58793E-1 * S(2 * (D - Mp))
        + E * .57212E-1 * S(2 * D - M - Mp)
        + .5332E-1 * S(2 * D + Mp)
        + E * .45874E-1 * S(2 * D - M)
        + E * .41024E-1 * S(Mp - M) - .34718E-1 * S(D)
        - E * .30465E-1 * S(M + Mp) + .15326E-1 * S(2 * (D - F))
        - .12528E-1 * S(2 * F - Mp) - .1098E-1 * S(2 * F - Mp)
        + .10674E-1 * S(4 * D - Mp) + .10034E-1 * S(3 * Mp)

        + .8548E-2 * S(4 * D - 2 * Mp)
        - E * .791E-2 * S(M - Mp + 2 * D)
        - E * .6783E-2 * S(2 * D + M) + .5162E-2 * S(Mp - D)
        + E * .5E-2 * S(M + D)
        + E * .4049E-2 * S(Mp - M + 2 * D)
        + .3996E-2 * S(2 * (Mp + D)) + .3862E-2 * S(4 * D)
        + .3665E-2 * S(2 * D - 3 * Mp)
        + E * .2695E-2 * S(2 * Mp - M)
        + .2602E-2 * S(Mp - 2 * F - 2 * D)
        + E * .2396E-2 * S(2 * D - M - 2 * Mp) - .2349E-2 * S(Mp + D)
        + E2 * .2249E-2 * S(2 * (D - M))
        - E * .2125E-2 * S(2 * Mp + M) - E2 * .2079E-2 * S(2 * M)
        + E2 * .2059E-2 * S(2 * D - Mp - 2 * M)
        - .1773E-2 * S(Mp + 2 * D - 2 * F)
        - .1595E-2 * S(2 * (F + D))
        + E * .122E-2 * S(4 * D - M - Mp)
        - .111E-2 * S(2 * (Mp + F)) + .892E-3 * S(Mp - 3 * D)
        - E * .811E-3 * S(M + Mp + 2 * D)
        + E * .761E-3 * S(4 * D - M - 2 * Mp)
        + E2 * .717E-3 * S(Mp - 2 * M)
        + E2 * .704E-3 * S(Mp - 2 * M - 2 * D)
        + E * .693E-3 * S(M - 2 * Mp + 2 * D)
        + E * .598E-3 * S(2 * D - M - 2 * F) + .55E-3 * S(Mp + 4 * D)
        + .538E-3 * S(4 * Mp) + E * .521E-3 * S(4 * D - M)
        + .486E-3 * S(2 * Mp - D));
    double B = (5.128189 * S(F) + .280606 * S(Mp + F)
        + .277693 * S(Mp - F) + .173238 * S(2 * D - F)
        + .55413E-1 * S(2 * D + F - Mp)
        + .46272E-1 * S(2 * D - F - Mp)
        + .32573E-1 * S(2 * D + F) + .17198E-1 * S(2 * Mp + F)
        + .9267E-2 * S(2 * D + Mp - F) + .8823E-2 * S(2 * Mp - F)
        + E * .8247E-2 * S(2 * D - M - F)
        + .4323E-2 * S(2 * D - F - 2 * Mp)
        + .42E-2 * S(2 * D + F + Mp)
        + E * .3372E-2 * S(F - M - 2 * D)
        + E * .2472E-2 * S(2 * D + F - M - Mp)
        + E * .2222E-2 * S(2 * D + F - M)
        + E * .2072E-2 * S(2 * D - F - M - Mp)
        + E * .1877E-2 * S(F - M + Mp)
        + .1828E-2 * S(4 * D - F - Mp)
        - E * .1803E-2 * S(F + M) - .175E-2 * S(3 * F)
        + E * .157E-2 * S(Mp - M - F) - .1487E-2 * S(F + D)
        - E * .1481E-2 * S(F + M + Mp)
        + E * .1417E-2 * S(F - M - Mp)
        + E * .135E-2 * S(F - M) + .133E-2 * S(F - D)
        + .1106E-2 * S(F + 3 * Mp) + .102E-2 * S(4 * D - F)
        + .833E-3 * S(F + 4 * D - Mp) + .781E-3 * S(Mp - 3 * F)
        + .67E-3 * S(F + 4 * D - 2 * Mp) + .606E-3 * S(2 * D - 3 * F)
        + .597E-3 * S(2 * D + 2 * Mp - F)
        + E * .492E-3 * S(2 * D + Mp - M - F)
        + .45E-3 * S(2 * Mp - F - 2 * D) + .439E-3 * S(3 * Mp - F)
        + .423E-3 * S(F + 2 * D + 2 * Mp)
        + .422E-3 * S(2 * D - F - 3 * Mp)
        - E * .367E-3 * S(M + F + 2 * D - Mp)
        - E * .353E-3 * S(M + F + 2 * D) + .331E-3 * S(F + 4 * D)
        + E * .317E-3 * S(2 * D + F - M + Mp)
        + E2 * .306E-3 * S(2 * D - 2 * M - F)
        - .283E-3 * S(Mp + 3 * F));
    double omega1 = .4664E-3 * C(omega);
    double omega2 = .754E-4 * C(omega + 275.05 - 2.3 * T);
    double beta = B * (1 - omega1 - omega2);
    double parallax = (.9507234 + .51818E-1 * C(Mp)
        + .9531E-2 * C(2 * D - Mp) + .7843E-2 * C(2 * D)
        + .2824E-2 * C(2 * Mp) + .857E-3 * C(2 * D + Mp)
        + E * .533E-3 * C(2 * D - M)
        + E * .401E-3 * C(2 * D - M - Mp)
        + E * .32E-3 * C(Mp - M) - .271E-3 * C(D)
        - .264E-3 * C(M + Mp) - .198E-3 * C(2 * F - Mp)
        + .173E-3 * C(3 * Mp) + .167E-3 * C(4 * D - Mp)
        - E * .111E-3 * C(M) + .103E-3 * C(4 * D - 2 * Mp)
        - .84E-4 * C(2 * (Mp - D)) - E * .83E-4 * C(2 * D + M)
        + .79E-4 * C(2 * (D + Mp)) + .72E-4 * C(4 * D)
        + E * .64E-4 * C(2 * D - M + Mp)
        - E * .63E-4 * C(2 * D + M - Mp)
        + E * .41E-4 * C(M + D) + E * .35E-4 * C(2 * Mp - M)
        - .33E-4 * C(3 * Mp - 2 * D) - .3E-4 * C(Mp + D)
        - .29E-4 * C(2 * (F - D)) - E * .29E-4 * C(2 * Mp + M)
        + E2 * .26E-4 * C(2 * (D - M))
        - .23E-4 * C(2 * F - 2 * D + Mp)
        + E * .19E-4 * C(4 * D - M - Mp));

    moonlon = lambda * (M_PI / 180);
    moonlat = beta * (M_PI / 180);
    moondist = 1 / S(parallax);
}

/* ------------------------------------------------------------------------*/

void MoonPos::getMoonPos(time_t ssue, double* lat, double* lon)
{
    double moonlon, moonlat, moondist;
    double moonalpha, moondelta;
    static double eps, T;
    static bool first_time = true;

    if (first_time) {
        T = jcentury(ssue);
        eps = compute_obliquity(T);
        first_time = false;
    }

    moonpos(moonlon, moonlat, moondist, jcentury(ssue));
    compute_ra_dec(moonlon, moonlat, moonalpha, moondelta, eps);
    *lat = moondelta / (M_PI / 180);
    *lon = 2 * M_PI * (moonalpha - gmst(T, ssue)) / 24 / (M_PI / 180);
}
