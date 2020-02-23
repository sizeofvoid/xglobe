/*
 * main.cpp
 *
 * $Id: main.cpp,v 1.2 2000/06/15 09:43:29 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: main.cpp,v $
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.3  1999/07/13 17:32:52  thorsten
 * *** empty log message ***
 *
 * Revision 0.2  1999/01/19 19:33:20  thorsten
 * Corrected the name of qimageio.h
 *
 * Revision 0.1  1998/12/08 19:15:51  thorsten
 * initial revision
 *
 */

#include "earthapp.h"

int main(int argc, char** argv)
{
    EarthApplication app(argc, argv);
    app.init();
    return app.exec();
}
