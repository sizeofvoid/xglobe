#include "geo_coordinate.h"

#include <cmath>
#include <sys/time.h>

void OrbitCoordinate::computePosition(time_t ssue)
{
    double x, y, z;
    double a, c, s;
    double t1, t2, shift;

    /* start at 0 N 0 E */
    x = 0;
    y = 0;
    z = 1;

    /* rotate in about y axis (from z towards x) according to the number
   * of orbits we've completed
   */
    a = (((double)ssue) / orbit_period) * (2 * M_PI);
    shift = fmod((a * orbit_shift), 360);
    c = cos(a);
    s = sin(a);
    t1 = c * z - s * x;
    t2 = s * z + c * x;
    z = t1;
    x = t2;

    /* rotate about z axis (from x towards y) according to the
   * inclination of the orbit
   */
    a = orbit_inclin * (M_PI / 180);
    c = cos(a);
    s = sin(a);
    t1 = c * x - s * y;
    t2 = s * x + c * y;
    x = t1;
    y = t2;

    /* rotate about y axis (from x towards z) according to the number of
   * rotations the earth has made
   */
    a = ((double)ssue / 86400) * (2 * M_PI);
    c = cos(a);
    s = sin(a);
    t1 = c * x - s * z;
    t2 = s * x + c * z;
    x = t1;
    z = t2;

    setLatitude(asin(y) * (180 / M_PI));
    setLongitude(atan2(x, z) * (180 / M_PI));
    if (getLongitude() + shift > 180)
        setLongitude(-180 + (shift - (180 - getLongitude())));
    else
        setLongitude(getLongitude() + shift);
}

