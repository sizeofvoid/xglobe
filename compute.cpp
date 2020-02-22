#include "compute.h"
#include <math.h>

RotMatrix::RotMatrix(double rot, double lon, double lat, double radius)
{
	const double crot = cos(rot) * radius;
	const double srot = sin(rot) * radius;
	const double clon = cos(lon);
	const double slon = sin(lon);
	const double clat = cos(lat);
	const double slat = sin(lat);

	m11 = crot * clon - slat * slon * srot;
	m12 = srot * clon - slat * slon * crot;
	m13 = clat * slon * radius;
	m21 = srot * clat;
	m22 = clat * crot;
	m23 = slat * radius;
	m31 = -slon * crot - slat * clon * srot;
	m32 = slon * srot - slat * clon * crot;
	m33 = clat * clon * radius;
}

void RotMatrix::transpose()
{
	swap(m12, m21);
	swap(m13, m31);
	swap(m23, m32);
}
