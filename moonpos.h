/*
 * moonpos.h
 *
 * $Id: moonpos.h,v 1.2 2000/06/15 09:43:29 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: moonpos.h,v $
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.1  1999/07/19 12:41:38  thorsten
 * initial revision
 *
 *
 */

#ifndef _MOONPOS_H
#define _MOONPOS_H

#include <qobject.h>
#include <math.h>
#include <time.h>

class MoonPos
{
private:
  static void moonpos(double &moonlon, double &moonlat, double &moondist,
                      double T);
  static double gmst(double T, time_t);
  static double C(double);
  static double S(double);
  static void compute_ra_dec(double lon, double lat, double &alpha,
                             double &delta, double eps);
  static double compute_obliquity(double T);
  static double poly(double a1, double a2, double a3, double a4, double t);
  static double jcentury(time_t);
  static double julian(int year, int month, int day, int hour, int min,
                       int sec);
  MoonPos() {}
  
public:
  static void getMoonPos(time_t ssue, double *lat, double *lon);
  ~MoonPos() {}
  
};

#endif
