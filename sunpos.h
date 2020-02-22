/*
 * sunpos.h
 *
 * $Id: sunpos.h,v 1.2 2000/06/15 09:43:30 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: sunpos.h,v $
 * Revision 1.2  2000/06/15 09:43:30  espie
 * Bump to what I have
 *
 * Revision 0.1  1998/12/09 18:51:11  thorsten
 * initial revision
 *
 */

#ifndef _SUNPOS_H
#define _SUNPOS_H

#include <qobject.h>
#include <time.h>

class SunPos  
{
private:
  static double solve_keplers_equation(double M);
  static double sun_ecliptic_longitude(time_t ssue);
  static void   ecliptic_to_equatorial(double lambda, double beta,
                                       double *alpha, double *delta);
  static double julian_date(int y, int m, int d);
  static double GST(time_t ssue);
  SunPos() {}

public:
  static void GetSunPos(time_t ssue, double *lat, double *lon);
  ~SunPos() {}
};

#endif
