/* 
 * random.h
 *
 * $Id: random.h,v 1.1 2000/06/15 09:43:29 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 2000 Marc Espie
 *
 */

#ifndef _RANDOM_H
#define _RANDOM_H
/* Singleton random generator class */

class Gen
{
public:
	Gen();	// dummy constructor, relies on initialized instead
	int operator ()(int max) const;
	double gaussian();
private:
	static bool initialized;
	inline int draw() const;
	// gaussian generator algorithm draws 2 numbers each time
	static bool has_one;
	static double preserve;
};

#endif
