#include "problem_solver.h"

#include <omp.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <utility>

std::ostream& operator<<(std::ostream& os, const Selection& selection)
{
	switch (selection)
	{
	case Selection::RANK:
		return os << "Rank";
	case Selection::ROULETTE_WHEEL:
		return os << "Roulette Wheel";
	case Selection::TOURNAMENT:
		return os << "Tournament";
	}

	return os;
}

void ProblemSolver::readData(const std::string& fileName)
{
	std::ifstream file(fileName);

	file >> B >> L >> D;

	for (std::uint32_t i = 0; i < B; i++)
	{
		std::uint16_t score; file >> score;
		books.push_back(score);
	}

	for (std::uint32_t i = 0; i < L; i++)
	{
		Library library;
		std::uint32_t bookCount;

		file >> bookCount >> library.signupTime >> library.bookScansPerDay;

		for (std::uint32_t j = 0; j < bookCount; j++)
		{
			std::uint32_t bookID; file >> bookID;
			library.books.push_back(bookID);

			std::sort(library.books.begin(), library.books.end(), [this](const std::uint32_t& a, const std::uint32_t& b)
			{
				return books[a] > books[b];
			});
		}

		libraries.push_back(library);
	}

	file.close();
}

void ProblemSolver::solve(Selection selectionMethod)
{
	const auto t1 = std::chrono::high_resolution_clock::now();

	Population population = generateInitialPopulation();
	bestScore = std::numeric_limits<std::uint64_t>::min();

	for (std::uint64_t generation = 0; generation <= generations; generation++)
	{
		std::vector<std::uint64_t> scores(populationSize);
		std::uint64_t totalScore = 0;

		#pragma omp parallel
		{
			Individual bestSolutionPrivate;
			std::uint64_t bestScorePrivate = bestScore;
			
			// calculate fitness for each individual in current population
			#pragma omp for reduction(+ : totalScore) nowait
			for (std::uint64_t i = 0; i < populationSize; i++)
			{
				const std::uint64_t score = calculateScore(population[i]);

				if (score > bestScore)
				{
					bestSolutionPrivate = population[i];
					bestScorePrivate = score;
				}

				scores[i] = score;
				totalScore += score;
			}

			#pragma omp critical
			{
				if (bestScorePrivate > bestScore)
				{
					bestSolution = bestSolutionPrivate;
					bestScore = bestScorePrivate;
				}
			}
		}

		// generate next generation using genetic operators:
		// selection, crossover and mutation
		switch (selectionMethod)
		{
		case Selection::RANK:
			population = rank(population);
			break;
		case Selection::ROULETTE_WHEEL:
			population = rouletteWheel(population, scores, totalScore);
			break;
		case Selection::TOURNAMENT:
			population = tournament(population, scores);
			break;
		}
	}

	selection = selectionMethod;

	const auto t2 = std::chrono::high_resolution_clock::now();
	executionTime = t2 - t1;
}

void ProblemSolver::writeSolution(const std::string& fileName) const
{
	const auto write = [this](std::ostream& os)
	{
		constexpr std::uint8_t width = 30;
		constexpr std::uint8_t precision = 2;

		os << std::fixed << std::setprecision(precision);

		os << "###################################################################\n";
		os << "#  Genetic algorithm solution for HashCode book scanning problem  #\n";
		os << "###################################################################\n";
		os << "######################  Algorithm parameters  #####################\n";
		os << "###################################################################\n";

		os << std::left << std::setw(width) << "Population size"       << populationSize << '\n';
		os << std::left << std::setw(width) << "Number of generations" << generations    << '\n';
		os << std::left << std::setw(width) << "Crossover rate"        << crossoverRate  << '\n';
		os << std::left << std::setw(width) << "Mutation rate"         << mutationRate   << '\n';
		os << std::left << std::setw(width) << "Elite pick percentage" << static_cast<std::uint16_t>(elitePercent * 100.0) << "%\n";
		os << std::left << std::setw(width) << "Selection method"      << selection << '\n';

		os << "###################################################################\n";
		os << "########################  Problem data set  #######################\n";
		os << "###################################################################\n";

		os << std::left << std::setw(width) << "Number of books"     << B << '\n';
		os << std::left << std::setw(width) << "Number of libraries" << L << '\n';
		os << std::left << std::setw(width) << "Number of days"      << D << '\n';

		os << "###################################################################\n";
		os << "#########################  Final results  #########################\n";
		os << "###################################################################\n";

		os << std::left << std::setw(width) << "Best score" << bestScore << '\n';
		os << std::left << std::setw(width) << "Execution time" << executionTime.count() / 1000.0 << " seconds\n";

		os << "###################################################################\n";
	};

	std::ofstream file(fileName, std::ofstream::trunc);
	
	write(file);
	write(std::cout);

	file.close();
}

std::uint64_t ProblemSolver::calculateScore(const Individual& individual) const
{
	std::uint64_t score = 0;
	std::unordered_set<std::uint32_t> scannedBooks;
	std::unordered_set<std::uint32_t> signedLibraries;

	const std::vector<std::uint32_t>& libraryIDs = individual;

	std::vector<std::uint32_t> currentBook(L, 0);

	std::uint32_t lib = 0;
	std::uint32_t signupDay = libraries[libraryIDs[lib]].signupTime - 1;

	for (std::uint32_t day = signupDay; day < D; day++)
	{
		for (auto it = signedLibraries.begin(); it != signedLibraries.end();)
		{
			std::uint32_t ID = *it;
			Library library = libraries[ID];
			std::uint32_t bookCount = std::min(library.bookScansPerDay, static_cast<std::uint32_t>(library.books.size() - currentBook[ID]));
			
			if (bookCount == 0)
			{
				it = signedLibraries.erase(it);
				continue;
			}

			for (std::uint32_t i = 0; i < bookCount; i++)
			{
				const std::uint32_t bookID = libraries[ID].books[currentBook[ID]++];

				if (scannedBooks.count(bookID) == 0)
				{
					score += books[bookID];
					scannedBooks.insert(bookID);
				}
			}

			++it;
		}

		if (signedLibraries.empty())
		{
			if (signupDay == D) break;
			else day = signupDay;
		}

		if (day == signupDay)
		{
			signedLibraries.insert(libraryIDs[lib++]);
			if (lib < L) signupDay = day + libraries[libraryIDs[lib]].signupTime;
			else signupDay = D;
		}
	}

	return score;
}

Population ProblemSolver::generateInitialPopulation()
{
	const auto permute = [this](std::vector<std::uint32_t>& vector)
	{
		for (std::size_t i = vector.size() - 1; i > 0; i--)
		{
			std::uint32_t j = getRandomInt(i + 1);
			if (i != j) std::swap(vector[i], vector[j]);
		}
	};

	Population population;
	population.reserve(populationSize);

	std::vector<std::uint32_t> libraryIDs(L);
	std::iota(libraryIDs.begin(), libraryIDs.end(), 0);

	for (std::uint64_t i = 0; i < populationSize; i++)
	{
		population.push_back(libraryIDs);
		permute(libraryIDs);
	}

	return population;
}

Population ProblemSolver::rank(Population& population)
{
	Population next;
	next.reserve(populationSize);
	constexpr std::uint32_t max = static_cast<std::uint32_t>(elitePercent * populationSize);

	// too slow - redundant score calculations
	std::sort(population.begin(), population.end(), [this](const Individual& a, const Individual& b)
	{
		return calculateScore(a) > calculateScore(b);
	});

	for (std::uint64_t i = 0; i < populationSize; i += parentCount)
	{
		std::unordered_set<std::uint32_t> set;
		Parents parents;

		for (std::uint64_t j = 0; j < parentCount; j++)
		{
			std::uint32_t index = getRandomInt(max);
			while (set.count(index)) index = getRandomInt(max);

			set.insert(index);
			parents[j] = population[index];
		}

		Parents offspring = pmx(parents);

		for (std::uint64_t j = 0; j < parentCount; j++)
		{
			mutate(offspring[j]);
			next.push_back(offspring[j]);
		}
	}

	return next;
}

Population ProblemSolver::rouletteWheel(const Population& population, const std::vector<std::uint64_t>& scores, std::uint64_t totalScore)
{
	Population next;
	next.reserve(populationSize);

	std::vector<double> probabilities(populationSize);

	for (std::size_t i = 0; i < scores.size(); i++)
	{
		probabilities[i] = scores[i] * 1.0 / totalScore;
	}

	for (std::uint64_t i = 0; i < populationSize; i += parentCount)
	{
		Parents parents;

		for (std::size_t j = 0; j < parentCount; j++)
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

		Parents offspring = pmx(parents);

		for (std::uint64_t j = 0; j < parentCount; j++)
		{
			mutate(offspring[j]);
			next.push_back(offspring[j]);
		}
	}

	return next;
}

Population ProblemSolver::tournament(const Population& population, const std::vector<std::uint64_t>& scores)
{
	Population next;
	next.reserve(populationSize);

	for (std::uint64_t i = 0; i < populationSize; i += parentCount)
	{
		Parents parents;

		for (std::uint64_t j = 0; j < parentCount; j++)
		{
			std::uint32_t  a = getRandomInt(populationSize);
			std::uint32_t  b = getRandomInt(populationSize);
			while (a == b) b = getRandomInt(populationSize);

			parents[j] = scores[a] > scores[b] ? population[a] : population[b];
		}

		Parents offspring = pmx(parents);

		for (std::uint64_t j = 0; j < parentCount; j++)
		{
			mutate(offspring[j]);
			next.push_back(offspring[j]);
		}
	}

	return next;
}

Parents ProblemSolver::pmx(const Parents& parents)
{
	const auto crossover = [this](std::vector<std::uint32_t>& a, std::vector<std::uint32_t>& b)
	{
		if (getRandomDouble() <= crossoverRate)
		{
			std::uint32_t index = getRandomInt(static_cast<std::uint32_t>(a.size()));

			for (std::uint32_t i = 0; i <= index; i++)
			{
				auto ab = std::find(a.begin(), a.end(), b[i]);
				auto ba = std::find(b.begin(), b.end(), a[i]);

				std::swap(a[i], *ab);
				std::swap(b[i], *ba);
			}
		}
	};

	Parents offspring = parents;

	for (std::uint64_t i = 0; i < parentCount; i += 2)
	{
		crossover(offspring[i], offspring[i + 1]);
	}

	return offspring;
}

void ProblemSolver::mutate(Individual& individual)
{
	const auto mutation = [this](std::vector<std::uint32_t>& values)
	{
		if (getRandomDouble() <= mutationRate)
		{
			std::uint32_t  a = getRandomInt(static_cast<std::uint32_t>(values.size()));
			std::uint32_t  b = getRandomInt(static_cast<std::uint32_t>(values.size()));
			while (a == b) b = getRandomInt(static_cast<std::uint32_t>(values.size()));

			std::swap(values[a], values[b]);
		}
	};

	mutation(individual);
}