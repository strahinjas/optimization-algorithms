#ifndef _PROBLEM_SOLVER_H_
#define _PROBLEM_SOLVER_H_

#include <array>
#include <chrono>
#include <cstdint>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

struct Library
{
	std::uint32_t signupTime;
	std::uint32_t bookScansPerDay;
	std::unordered_set<std::uint32_t> books;
};

struct Individual
{
	std::vector<std::uint32_t> libraries;
	std::vector<std::vector<std::uint32_t>> books;
};

constexpr std::uint64_t populationSize = 10000;
constexpr std::uint64_t generations = 50;

constexpr double crossoverRate = 0.8;
constexpr double mutationRate  = 0.1;
constexpr double elitePercent  = 0.1;

constexpr std::uint64_t parentCount = 2;

using Population = std::vector<Individual>;
using Parents = std::array<Individual, parentCount>;

enum class Selection
{
	RANK,
	ROULETTE_WHEEL,
	TOURNAMENT
};

std::ostream& operator<<(std::ostream& os, const Selection& selection);

class ProblemSolver
{
public:
	void readData(const std::string& fileName);

	void solve(Selection selectionMethod);

	void writeSolution(const std::string& fileName) const;
private:
	std::uint64_t calculateScore(const Individual& individual) const;

	std::vector<Individual> generateInitialPopulation();

	// selection
	Population rank(Population& population);
	Population rouletteWheel(const Population& population, const std::vector<std::uint64_t>& scores, std::uint64_t totalScore);
	Population tournament(const Population& population, const std::vector<std::uint64_t>& scores);

	// partially-mapped crossover
	Parents pmx(const Parents& parents);

	// random swap mutation
	void mutate(Individual& individual);

	std::uint32_t getRandomInt(std::uint32_t max)
	{
		std::uniform_int_distribution<std::uint32_t> distribution(0, max - 1);
		return distribution(engine);
	}

	double getRandomDouble()
	{
		std::uniform_real_distribution<> distribution;
		return distribution(engine);
	}

	const std::chrono::system_clock::rep seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine engine = std::default_random_engine(static_cast<std::uint32_t>(seed));

	Selection selection;
	std::chrono::duration<double, std::milli> executionTime;

	std::uint32_t B;
	std::uint32_t L;
	std::uint32_t D;

	std::vector<std::uint16_t> books;
	std::vector<Library> libraries;

	Individual bestSolution;
	std::uint64_t bestScore;
};

#endif