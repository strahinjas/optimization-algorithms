#ifndef _CIRCUIT_BOARD_H_
#define _CIRCUIT_BOARD_H_

#include "point.h"

#include <vector>

class CircuitBoard
{
public:
	static const std::vector<Point>& getHoles()
	{
		return holes;
	}
private:
	CircuitBoard() = delete;

	CircuitBoard(const CircuitBoard&) = delete;
	CircuitBoard& operator=(const CircuitBoard&) = delete;

	static const std::vector<Point> holes;
};

#endif