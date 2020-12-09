#include <array>
#include <bitset>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <unordered_set>

constexpr std::int64_t FILE_COUNT = 64;

constexpr std::array<std::int64_t, FILE_COUNT> sizes =
{
	173669,  275487,  1197613, 1549805, 502334,  217684,  1796841, 274708,
	631252,  148665,  150254,  4784408, 344759,  440109,  4198037, 329673,
	28602,   144173,  1461469, 187895,  369313,  959307,  1482335, 2772513,
	1313997, 254845,  486167,  2667146, 264004,  297223,  94694,   1757457,
	576203,  8577828, 498382,  8478177, 123575,  4062389, 3001419, 196884,
	617991,  421056,  3017627, 131936,  1152730, 2676649, 656678,  4519834,
	201919,  56080,   2142553, 326263,  8172117, 2304253, 4761871, 205387,
	6148422, 414559,  2893305, 2158562, 465972,  304078,  1841018, 1915571
};

using bits = std::bitset<FILE_COUNT>;

constexpr std::int64_t MEMORY_SIZE  = 64 * 1024 * 1024;
constexpr std::int64_t AVERAGE_SIZE = 32 * 1024 * 1024;

constexpr std::int64_t RUN_COUNT = 20;
constexpr std::int64_t MAX_ITERATIONS = 100000;

constexpr double A = 0.95;

const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine engine(static_cast<unsigned int>(seed));

inline std::int64_t getRandomInt(std::int64_t max)
{
	std::uniform_int_distribution<std::int64_t> distribution(0, max);
	return distribution(engine);
}

inline double getRandomDouble()
{
	std::uniform_real_distribution<> distribution;
	return distribution(engine);
}

std::int64_t costFunction(const bits& files)
{
	std::int64_t cost = MEMORY_SIZE;

	for (std::int64_t i = 0; i < FILE_COUNT; i++)
	{
		cost -= (files[i] ? sizes[i] : 0);
	}

	return cost < 0 ? MEMORY_SIZE : cost;
}

bits getNext(const bits& files, std::int64_t distance)
{
	bits result = files;
	std::unordered_set<std::int64_t> set;

	if (distance == FILE_COUNT) return result.flip();

	while (distance-- > 0)
	{
		std::int64_t index = getRandomInt(FILE_COUNT - 1);
		while (set.count(index)) index = getRandomInt(FILE_COUNT - 1);

		result.flip(index);
		set.insert(index);
	}

	return result;
}

inline std::int64_t getNextDistance(std::int64_t i)
{
	return (1 - FILE_COUNT) * i / (MAX_ITERATIONS - 1) + FILE_COUNT;
}

inline double probability(std::int64_t deltaE, double T)
{
	return deltaE == 0 ? 0.5 : std::exp(-1.0 * deltaE / T);
}

std::array<std::array<std::int64_t, RUN_COUNT>, MAX_ITERATIONS> cumulativeMinimum = {};

int main(int argc, const char* argv[])
{
	bits globalOptimal;
	std::int64_t globalMinimum = std::numeric_limits<std::int64_t>::max();

	for (std::int64_t run = 0; run < RUN_COUNT; run++)
	{
		double T = AVERAGE_SIZE;

		bits optimal, files;
		std::int64_t minCost, lastCost;

		for (std::size_t i = 0; i < FILE_COUNT; i++)
		{
			files[i] = getRandomInt(1);
		}

		optimal = files;
		minCost = lastCost = costFunction(files);

		for (std::int64_t iteration = 0; iteration < MAX_ITERATIONS; iteration++)
		{
			std::int64_t distance = getNextDistance(iteration);

			bits next = getNext(files, distance);
			std::int64_t nextCost = costFunction(next);

			std::int64_t deltaE = nextCost - lastCost;

			if (deltaE < 0 || getRandomDouble() < probability(deltaE, T))
			{
				if (nextCost < minCost)
				{
					optimal = next;
					minCost = nextCost;
				}

				files = next;
				lastCost = nextCost;
			}

			cumulativeMinimum[iteration][run] = minCost;

			// exponential cooling
			T *= A;

			// logarithmic cooling
			// T = AVERAGE_SIZE * 1.0 / std::log(iteration + 3);
		}

		if (minCost < globalMinimum)
		{
			globalOptimal = optimal;
			globalMinimum = minCost;
		}
	}

	std::ofstream cumulative("cumulative_minimum.txt", std::ofstream::trunc);
	std::ofstream averageCumulative("average_cumulative_minimum.txt", std::ofstream::trunc);

	for (std::size_t i = 0; i < MAX_ITERATIONS; i++)
	{
		cumulative << i << ' ';
		averageCumulative << i << ' ';

		std::int64_t sum = 0;

		for (std::size_t j = 0; j < RUN_COUNT; j++)
		{
			sum += cumulativeMinimum[i][j];
			cumulative << cumulativeMinimum[i][j] << ' ';
		}

		cumulative << '\n';
		averageCumulative << sum / RUN_COUNT << '\n';
	}

	cumulative.close();
	averageCumulative.close();

	std::cout << "Files: " << globalOptimal << '\n';
	std::cout << "Minimal cost: " << globalMinimum << '\n';

	return 0;
}