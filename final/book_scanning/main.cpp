#include "problem_solver.h"

#include <iostream>
#include <string>

int main(int argc, const char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Invalid number of arguments!\n";
		std::cerr << "Missing input data set.\n";
		return 1;
	}

	const std::string inputFileName  = argv[1];
	const std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + "_solution.txt";

	ProblemSolver problemSolver;
	constexpr Selection selectionMethod = Selection::TOURNAMENT;

	// read input data
	problemSolver.readData(inputFileName);

	// solve problem using genetic algorithm
	problemSolver.solve(selectionMethod);

	// write best solution to the output file
	problemSolver.writeSolution(outputFileName);

	return 0;
}