#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>

constexpr double W  = 0.729;
constexpr double C1 = 1.494;
constexpr double C2 = 1.494;

constexpr std::uint64_t POPULATION_SIZE  = 2000;
constexpr std::uint64_t GENERATION_COUNT = 50;

struct point
{
	double x;
	double y;
	double z;
};

point operator+(const point& a, const point& b)
{
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

point operator-(const point& a, const point& b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

constexpr auto distance = [](const point& a, const point& b)
{
	return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) + std::pow(a.z - b.z, 2));
};

enum
{
	A, B, C, D, COUNT
};

constexpr std::array<point, COUNT> points
{{
	{ 1.0, 5.0, 1.0 }, { 3.0, 2.0, 0.0 },
	{ 5.0, 6.0, 1.0 }, { 6.0, 3.0, 3.0 }
}};

using particle = std::pair<point, point>;

template<typename T>
using array = std::array<T, POPULATION_SIZE>;

particle operator+(const particle& a, const particle& b)
{
	return std::make_pair(a.first + b.first, a.second + b.second);
}

particle operator-(const particle& a, const particle& b)
{
	return std::make_pair(a.first - b.first, a.second - b.second);
}

constexpr auto multiply = [](double c, const particle& p)
{
	point S1 = { c * p.first.x,  c * p.first.y,  c * p.first.z  };
	point S2 = { c * p.second.x, c * p.second.y, c * p.second.z };

	return std::make_pair(S1, S2);
};

const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine engine(static_cast<unsigned int>(seed));

constexpr auto randomDouble = [](double min, double max)
{
	std::uniform_real_distribution distribution(min, max);
	return distribution(engine);
};

double costFunction(const particle& particle)
{
	const point& S1 = particle.first;
	const point& S2 = particle.second;

	return distance(points[A], S1) + distance(points[B], S1) + distance(S1, S2) + distance(points[C], S2) + distance(points[D], S2);
}

void calculateVelocity(particle& velocity, const particle& particleBest, const particle& swarm, const particle& globalBestParticle)
{
	particle P1 = multiply(W, velocity);
	particle P2 = multiply(C1 * randomDouble(0.0, 1.0), particleBest - swarm);
	particle P3 = multiply(C2 * randomDouble(0.0, 1.0), globalBestParticle - swarm);

	velocity = P1 + P2 + P3;
}

int main(int argc, const char* argv[])
{
	particle globalBestParticle;
	array<particle> swarm, particleBest, velocity;

	array<double> localBestCost;
	localBestCost.fill(std::numeric_limits<double>::max());
	double globalBestCost = std::numeric_limits<double>::max();

	for (std::uint64_t i = 0; i < POPULATION_SIZE; i++)
	{
		point S1 = { randomDouble(0.0, 10.0), randomDouble(0.0, 10.0), randomDouble(0.0, 4.0) };
		point S2 = { randomDouble(0.0, 10.0), randomDouble(0.0, 10.0), randomDouble(0.0, 4.0) };

		swarm[i] = std::make_pair(S1, S2);

		point P1 = { 0.01, 0.01, 0.01 };
		point P2 = { 0.01, 0.01, 0.01 };

		velocity[i] = std::make_pair(P1, P2);
	}

	for (std::uint64_t generation = 0; generation < GENERATION_COUNT; generation++)
	{
		for (std::uint64_t i = 0; i < POPULATION_SIZE; i++)
		{
			const double cost = costFunction(swarm[i]);

			if (cost < localBestCost[i])
			{
				particleBest[i] = swarm[i];
				localBestCost[i] = cost;
			}

			if (cost < globalBestCost)
			{
				globalBestParticle = swarm[i];
				globalBestCost = cost;
			}

			calculateVelocity(velocity[i], particleBest[i], swarm[i], globalBestParticle);

			swarm[i] = swarm[i] + velocity[i];
		}
	}

	constexpr auto print = [](const char* s, const point& p)
	{
		std::cout << s << '(' << p.x << ", " << p.y << ", " << p.z << ")\n";
	};

	std::cout << std::fixed << std::setprecision(3);

	print("S1", globalBestParticle.first);
	print("S2", globalBestParticle.second);

	std::cout << "\nMinimal cost: " << globalBestCost << '\n';

	return 0;
}