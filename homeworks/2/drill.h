#ifndef _DRILL_H_
#define _DRILL_H_

#include "point.h"

#include <vector>

class Drill
{
public:
	void calculateShortestPath(const std::vector<Point>& holes, std::size_t count);
	void printResult() const;
private:
	std::vector<std::vector<double>> calculateDistanceMatrix(const std::vector<Point>& points) const;
	double calculateDistance(const std::vector<Point>& points, const std::vector<std::vector<double>>& distances) const;

	double minimalDistance;
	std::vector<Point> shortestPath;
};

#endif