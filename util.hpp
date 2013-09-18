#ifndef UTIL_H
#define UTIL_H

#include <tvmet/Vector.h>
#include <algorithm>

/* convenient vector typedef */
typedef tvmet::Vector<double, 3> vec3;

/* Return a vector restricted so it is no longer than vmax */
template <typename T, std::size_t N>
tvmet::Vector< T, N > restrict_vector(tvmet::Vector< T, N > V, double vmax)
{
	double v = tvmet::norm2(V);
	v = (v==0.0)?1.0:v;
	V = std::min(vmax, v) * V / v;
	return V;
}

#endif
