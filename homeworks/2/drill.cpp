#include "drill.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>

void Drill::calculateShortestPath(const std::vector<Point>& holes, std::size_t count)
{
	minimalDistance = std::numeric_limits<double>::max();
	shortestPath.clear();

	std::vector<Point> points;
	std::copy(holes.begin(), holes.begin() + count, std::back_inserter(points));

	std::vector<std::vector<double>> distances = calculateDistanceMatrix(points);

	do
	{
		double distance = calculateDistance(points, distances);

		if (distance < minimalDistance)
		{
			minimalDistance = distance;
			shortestPath = points;
		}
	}
	while (std::next_permutation(points.begin(), points.end()));
}

void Drill::printResult() const
{
	std::cout << "Total distance: " << minimalDistance << "mm\n\n";
	std::cout << "Shortest path:\n\n";

	for (const auto& point : shortestPath)
	{
		std::cout << point << '\n';
	}

	std::cout << '\n';
}

std::vector<std::vector<double>> Drill::calculateDistanceMatrix(const std::vector<Point>& points) const
{
	std::vector<std::vector<double>> distances;

	for (std::size_t i = 0; i < points.size(); i++)
	{
		distances.push_back(std::vector<double>(points.size(), 0.0));

		for (std::size_t j = 0; j < points.size(); j++)
		{
			if (i != j) distances[i][j] = euclideanDistance(points[i], points[j]);
		}
	}

	return distances;
}

double Drill::calculateDistance(const std::vector<Point>& points, const std::vector<std::vector<double>>& distances) const
{
	double distance = 0.0;

	for (std::size_t i = 0; i < points.size() - 1; i++)
	{
		distance += distances[points[i].getID() - 1][points[i + 1].getID() - 1];
	}

	return distance;
}