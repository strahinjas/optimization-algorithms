#ifndef _POINT_H_
#define _POINT_H_

#include <cmath>
#include <ostream>

class Point
{
public:
	Point(unsigned int ID, double x, double y) : ID(ID), x(x), y(y) {}

	unsigned int getID() const { return ID; }

	double getX() const { return x; }
	double getY() const { return y; }

	bool operator<(const Point& other) const
	{
		return ID < other.ID;
	}

	friend double euclideanDistance(const Point& p, const Point& q);

	friend std::ostream& operator<<(std::ostream& os, const Point& point);
private:
	unsigned int ID;

	double x;
	double y;
};

inline double euclideanDistance(const Point& p, const Point& q)
{
	return std::sqrt(std::pow(p.x - q.x, 2) + std::pow(p.y - q.y, 2));
}

#endif