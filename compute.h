#ifndef _COMPUTE_H
#define _COMPUTE_H

#ifndef PI
#define PI ((double)3.141592654)
#endif
#ifndef TWOPI
#define TWOPI (2*PI)
#endif

#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define MIN(x, y) ((x) > (y) ? (y) : (x))

class RotMatrix 
	{
public:
	RotMatrix(double rot, double lon, double lat, double radius = 1.0);
	inline void transform(double src_x, double src_y, double src_z, 
		double& dest_x, double& dest_y, double& dest_z) const
		{
			dest_x = m11 * src_x + m12 * src_y + m13 *src_z;
			dest_y = m21 * src_x + m22 * src_y + m23 *src_z;
			dest_z = m31 * src_x + m32 * src_y + m33 *src_z;
		}
	void transpose();
private:
	double m11, m12, m13, m21, m22, m23, m31, m32, m33;
	static inline void swap(double& a, double& b)
		{
		double c = a;
		a = b;
		b = c;
		}

	};

#endif
