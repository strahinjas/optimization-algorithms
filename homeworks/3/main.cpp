#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

const std::vector<std::vector<int>> costs =
{
	{ 0, 374, 200, 223, 108, 178, 252, 285, 240, 356 },
	{ 0, 255, 166, 433, 199, 135,  95, 136,  17 },
	{ 0, 128, 277, 821, 180, 160, 131, 247 },
	{ 0, 430,  47,  52,  84,  40, 155 },
	{ 0, 453, 478, 344, 389, 423 },
	{ 0,  91, 110,  64, 181 },
	{ 0, 114,  83, 117 },
	{ 0,  47,  78 },
	{ 0, 118 },
	{ 0 }
};

inline int linkCost(int i, int j)
{
	return (i < j ? costs[i][j - i] : costs[j][i - j]);
}

int optimisationFunction(const std::vector<int>& t)
{
	int cost = 0;
	std::vector<int> degrees(t.size() / 2 + 1, 0);

	for (std::size_t i = 0; i < t.size() - 1; i += 2)
	{
		degrees[t[i]]++;
		degrees[t[i + 1]]++;

		cost += linkCost(t[i], t[i + 1]);
	}

	for (std::size_t i = 0; i < degrees.size(); i++)
	{
		if (degrees[i] >= 4)
		{
			cost += 100 * (degrees[i] - 3);
		}
	}

	return cost;
}

void sequenceToSpanningTree(const std::vector<int>& p, std::vector<int>& t)
{
	std::size_t q = 0;
	std::size_t n = p.size() + 2;

	std::vector<int> v(n, 0);

	for (std::size_t i = 0; i < p.size(); i++) v[p[i]]++;

	for (std::size_t i = 0; i < p.size(); i++)
	{
		for (std::size_t j = 0; j < n; j++)
		{
			if (v[j] == 0)
			{
				v[j] = -1;
				t[q++] = j;
				t[q++] = p[i];
				v[p[i]]--;
				break;
			}
		}
	}

	std::size_t j = 0;

	for (std::size_t i = 0; i < n; i++)
	{
		if (v[i] == 0 && j == 0)
		{
			t[q++] = i;
			j++;
		}
		else if (v[i] == 0 && j == 1)
		{
			t[q++] = i;
			break;
		}
	}
}

void findOptimalSpanningTree(int n, int k)
{
	int q;
	std::vector<int> p(k, 0);

	int minimalCost = std::numeric_limits<int>::max();
	std::vector<int> optimalTree;

	do
	{
		std::vector<int> t(2 * (n - 1));

		sequenceToSpanningTree(p, t);
		int cost = optimisationFunction(t);

		if (cost < minimalCost)
		{
			minimalCost = cost;
			optimalTree = t;
		}

		q = k - 1;

		while (q >= 0)
		{
			if (++p[q] == n)
			{
				p[q] = 0;
				--q;
			}
			else break;
		}
	}
	while (q >= 0);

	auto cityName = [](int i) -> char { return 'A' + i; };

	std::cout << "Optimal Spanning Tree:\n\n";

	for (std::size_t i = 0; i < optimalTree.size() - 1; i += 2)
	{
		std::cout << '(' << cityName(optimalTree[i]) << " - " << cityName(optimalTree[i + 1]) << ")\n";
	}

	std::cout << "\nMinimal cost: " << minimalCost << '\n';
}

int main()
{
	const std::size_t n = costs[0].size();

	const auto t1 = std::chrono::high_resolution_clock::now();

	findOptimalSpanningTree(n, n - 2);

	const auto t2 = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double, std::milli> duration = t2 - t1;

	std::cout << "Computation time: " << duration.count() << "ms\n";
}