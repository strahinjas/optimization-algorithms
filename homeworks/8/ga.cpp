#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
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

constexpr std::int64_t MEMORY_SIZE = 64 * 1024 * 1024;
constexpr std::int64_t POPULATION_SIZE = 2000;
constexpr double DECIMATION_PERCENTAGE = 0.1;

using bits = std::bitset<FILE_COUNT>;

template<typename T>
using array = std::array<T, POPULATION_SIZE>;

constexpr std::int64_t RUN_COUNT = 20;
constexpr std::int64_t MAX_ITERATIONS = 200000;
constexpr std::int64_t GENERATION_COUNT = MAX_ITERATIONS / POPULATION_SIZE;

constexpr double CROSSOVER_RATE = 0.8;
constexpr double MUTATION_RATE  = 0.15;

constexpr std::int64_t STOP_COST = 32;
constexpr std::size_t PARENT_COUNT = 2;

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

array<bits> generateInitialPopulation()
{
	array<bits> population;

	const std::size_t populationSize = population.size();
	const std::size_t individualSize = population[0].size();

	for (std::size_t i = 0; i < populationSize; i++)
	{
		for (std::size_t j = 0; j < individualSize; j++)
		{
			population[i][j] = getRandomInt(1);
		}
	}

	return population;
}

std::array<std::array<std::int64_t, RUN_COUNT>, MAX_ITERATIONS> cumulativeMinimum = {};

int main(int argc, const char* argv[])
{
	bits globalOptimal;
	std::int64_t globalMinimum = std::numeric_limits<std::int64_t>::max();

	for (std::int64_t run = 0; run < RUN_COUNT; run++)
	{
		array<bits> population = generateInitialPopulation();

		bits optimal;
		std::int64_t minCost = std::numeric_limits<std::int64_t>::max();

		for (std::int64_t generation = 0; generation < GENERATION_COUNT; generation++)
		{
			array<bits> nextGeneration;
#ifdef USE_ROULETTE
			array<std::int64_t> costs;
			array<double> probabilities;

			double totalCost = 0.0;

			for (std::size_t i = 0; i < population.size(); i++)
			{
				const std::int64_t cost = costFunction(population[i]);

				costs[i] = cost;
				totalCost += 1.0 / cost;

				if (cost < minCost)
				{
					minCost = cost;
					optimal = population[i];
				}

				cumulativeMinimum[generation * population.size() + i][run] = minCost;
			}

			if (minCost <= STOP_COST)
			{
				for (std::int64_t i = generation + 1; i < GENERATION_COUNT; i++)
				{
					for (std::size_t j = 0; j < population.size(); j++)
					{
						cumulativeMinimum[i * population.size() + j][run] = minCost;
					}
				}

				break;
			}

			for (std::size_t i = 0; i < costs.size(); i++)
			{
				probabilities[i] = 1.0 / (costs[i] * totalCost);
			}
#else
			for (std::size_t i = 0; i < population.size(); i++)
			{
				const std::int64_t cost = costFunction(population[i]);

				if (cost < minCost)
				{
					minCost = cost;
					optimal = population[i];
				}

				cumulativeMinimum[generation * population.size() + i][run] = minCost;
			}

			std::sort(population.begin(), population.end(), [](const bits& a, const bits& b)
			{
				return costFunction(a) < costFunction(b);
			});

			std::int64_t cost = costFunction(population[0]);

			if (cost < minCost)
			{
				minCost = cost;
				optimal = population[0];
			}
#endif
			for (std::size_t i = 0; i < population.size(); i += PARENT_COUNT)
			{
				bits parents[PARENT_COUNT];
				bits offspring[PARENT_COUNT];
#ifdef USE_ROULETTE
				// roulette wheel selection
				for (std::size_t j = 0; j < PARENT_COUNT; j++)
				{
					double probability = getRandomDouble();
					double sum = 0.0;

					for (std::size_t k = 0; k < probabilities.size(); k++)
					{
						sum += probabilities[k];
						if (probability <= sum)
						{
							parents[j] = population[k];
							break;
						}
					}
				}
#else
				std::unordered_set<std::int64_t> set;

				for (std::size_t j = 0; j < PARENT_COUNT; j++)
				{
					std::int64_t index = getRandomInt(DECIMATION_PERCENTAGE * POPULATION_SIZE - 1);
					while (set.count(index)) index = getRandomInt(DECIMATION_PERCENTAGE * POPULATION_SIZE - 1);

					parents[j] = population[index];
					set.insert(index);
				}
#endif
				if (getRandomDouble() <= CROSSOVER_RATE)
				{
					// single-point crossover
					for (std::size_t j = 0; j < PARENT_COUNT; j += 2)
					{
						std::int64_t index = getRandomInt(FILE_COUNT - 1);

						for (std::size_t k = 0; k < FILE_COUNT; k++)
						{
							offspring[j][k]     = (k <= index ? parents[j][k]     : parents[j + 1][k]);
							offspring[j + 1][k] = (k <= index ? parents[j + 1][k] : parents[j][k]);
						}
					}
				}
				else
				{
					for (std::size_t j = 0; j < PARENT_COUNT; j++)
					{
						offspring[j] = parents[j];
					}
				}

				for (std::size_t j = 0; j < PARENT_COUNT; j++)
				{
					// flip random bit mutation
					if (getRandomDouble() <= MUTATION_RATE)
					{
						offspring[j].flip(getRandomInt(FILE_COUNT - 1));
					}

					nextGeneration[i + j] = offspring[j];
				}
			}

			population = nextGeneration;
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