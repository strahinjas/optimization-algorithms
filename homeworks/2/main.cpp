#include "drill.h"
#include "circuit_board.h"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>

int main()
{
	constexpr std::size_t A = 8;
	constexpr std::size_t B = 12;

	Drill drill;

	auto calculate = [&drill](std::size_t count)
	{
		std::cout << std::fixed << std::setprecision(0);
		std::cout << "Number of points: " << count << " (" << std::tgamma(count + 1.0) << " possible paths)\n\n";
		std::cout << std::fixed << std::setprecision(5);

		const auto t1 = std::chrono::high_resolution_clock::now();

		drill.calculateShortestPath(CircuitBoard::getHoles(), count);

		const auto t2 = std::chrono::high_resolution_clock::now();
		const std::chrono::duration<double, std::milli> duration = t2 - t1;

		drill.printResult();

		std::cout << std::fixed << std::setprecision(5);
		std::cout << "Computation time: " << duration.count() << "ms\n\n";
	};

	calculate(A);
	calculate(B);

	return 0;
}