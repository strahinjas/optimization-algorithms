#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <unordered_set>

constexpr std::uint64_t WEIGHT_COUNT = 10;

constexpr std::uint64_t POPULATION_SIZE  = 100;
constexpr std::uint64_t GENERATION_COUNT = 50;

constexpr double F  = 0.8;
constexpr double CR = 0.9;
constexpr double PI = 3.141592653589793;

constexpr double STOP_COST = 1.0e-2;

constexpr double MIN_WEIGHT = -10.0;
constexpr double MAX_WEIGHT =  10.0;

constexpr std::uint64_t A = 0;
constexpr std::uint64_t B = 1;
constexpr std::uint64_t C = 2;
constexpr std::uint64_t PICKS = 3;

using weights = std::array<double, WEIGHT_COUNT>;

template<typename T>
using array = std::array<T, POPULATION_SIZE>;

const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine engine(static_cast<unsigned int>(seed));

constexpr auto randomInt = [](std::uint64_t max)
{
	std::uniform_int_distribution<std::uint64_t> distribution(0, max - 1);
	return distribution(engine);
};

constexpr auto randomDouble = [](double min, double max)
{
	std::uniform_real_distribution distribution(min, max);
	return distribution(engine);
};

double targetFunction(double x)
{
	return 0.5 * std::sin(PI * x);
}

double output(double x, weights weights)
{
	double sum = 0.0;

	for (std::uint64_t i = 0; i < 5; i++)
	{
		sum += weights[i + 5] * std::tanh(weights[i] * x);
	}

	return std::tanh(sum);
}

double costFunction(weights weights)
{
	double cost = 0.0;

	for (double x = -1.0; x <= 1.0; x += 0.1)
	{
		cost += std::pow(output(x, weights) - targetFunction(x), 2);
	}

	return std::sqrt(cost);
}

int main(int argc, const char* argv[])
{
	array<weights> population;

	for (std::size_t i = 0; i < population.size(); i++)
	{
		for (std::size_t j = 0; j < population[0].size(); j++)
		{
			population[i][j] = randomDouble(MIN_WEIGHT, MAX_WEIGHT);
		}
	}

	weights solution;
	double minimalCost = std::numeric_limits<double>::max();

	while (true)
	{
		array<double> costs;

		for (std::size_t i = 0; i < population.size(); i++)
		{
			const double cost = costFunction(population[i]);

			if (cost < minimalCost)
			{
				solution = population[i];
				minimalCost = cost;
			}

			costs[i] = cost;
		}

		if (minimalCost < STOP_COST) break;

		array<weights> candidates;
		for (std::size_t i = 0; i < population.size(); i++)
		{
			std::unordered_set<std::size_t> set;
			std::array<weights, PICKS> x;

			set.insert(i);
			for (std::size_t j = 0; j < PICKS; j++)
			{
				std::size_t index = randomInt(population.size());
				while (set.count(index)) index = randomInt(population.size());
				
				set.insert(index);
				x[j] = population[index];
			}

			const std::uint64_t R = randomInt(WEIGHT_COUNT);

			for (std::size_t j = 0; j < WEIGHT_COUNT; j++)
			{
				const double z = x[A][j] + F * (x[B][j] - x[C][j]);

				if (randomDouble(0.0, 1.0) < CR || j == R)
				{
					candidates[i][j] = z;
				}
				else
				{
					candidates[i][j] = population[i][j];
				}
			}
		}

		for (std::size_t i = 0; i < population.size(); i++)
		{
			if (costFunction(candidates[i]) < costs[i]) population[i] = candidates[i];
		}
	}

	constexpr std::uint8_t WIDTH = 6;
	constexpr std::uint8_t PRECISION = 15;

	std::cout << "Weights:\n";
	std::cout << std::fixed << std::setprecision(PRECISION);

	for (std::size_t i = 0; i < WEIGHT_COUNT; i++)
	{
		std::cout << std::left << std::setw(WIDTH) << "w[" << i + 1 << ']';
		std::cout << "= " << solution[i] << '\n';
	}

	std::cout << "Minimal cost: " << minimalCost << '\n';

	return 0;
}