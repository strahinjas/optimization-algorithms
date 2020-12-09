#include "point.h"

#include <iomanip>

constexpr short PRECISION = 1;

std::ostream& operator<<(std::ostream& os, const Point& point)
{
	os << std::fixed << std::setprecision(PRECISION);
	os << '#' << point.ID << '(' << point.x << ", " << point.y << ')';
	return os;
}