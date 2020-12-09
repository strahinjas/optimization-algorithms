#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

constexpr double PRICE = 7.11;

inline bool check(int a, int b, int c, int d)
{
	double fa = a / 100.0;
	double fb = b / 100.0;
	double fc = c / 100.0;
	double fd = d / 100.0;

	if ((fa + fb + fc + fd) == (fa * fb * fc * fd) && (fa + fb + fc + fd) == PRICE)
	{
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Prices: " << fa << ' ' << fb << ' ' << fc << ' ' << fd << '\n';
		return true;
	}

	return false;
}

void searchA()
{
	constexpr int price = PRICE * 100;

	std::cout << std::fixed << std::setprecision(0);
	std::cout << "Maximum number of calls: " << std::pow(price, 4) << '\n';

	for (int a = 0; a <= price; a++)
		for (int b = 0; b <= price; b++)
			for (int c = 0; c <= price; c++)
				for (int d = 0; d <= price; d++)
				{
					if (check(a, b, c, d)) return;
				}
}

void searchB()
{
	constexpr int price = PRICE * 100;

	std::cout << std::fixed << std::setprecision(0);
	std::cout << "Maximum number of calls: " << std::pow(price, 3) << '\n';

	for (int a = 0; a <= price; a++)
		for (int b = 0; b <= price; b++)
			for (int c = 0; c <= price; c++)
			{
				if (check(a, b, c, price - (a + b + c))) return;
			}
}

void searchC()
{
	std::vector<double> factors;

	auto round = [](double d) { return d / 100.0; };

	constexpr int price = PRICE * 1e8;

	for (int i = 2; i < std::sqrt(price) + 1; i++)
	{
		if (price % i == 0)
		{
			const double fa = round(price / i);
			const double fb = round(i);

			if (fa < PRICE) factors.push_back(fa);
			if (fb < PRICE) factors.push_back(fb);
		}
	}

	std::cout << "Maximum number of calls: " << factors.size() * factors.size() * factors.size() << '\n';

	for (std::size_t i = 0; i < factors.size(); i++)
		for (std::size_t j = i; j < factors.size(); j++)
			for (std::size_t k = j; k < factors.size(); k++)
			{
				double fa = factors[i];
				double fb = factors[j];
				double fc = factors[k];
				double fd = PRICE - fa - fb - fc;

				if (fa * fb * fc * fd == PRICE)
				{
					std::cout << std::fixed << std::setprecision(2);
					std::cout << "Prices: " << fa << ' ' << fb << ' ' << fc << ' ' << fd << '\n';
					return;
				}
			}
}

int main()
{
	auto search = [](auto function)
	{
		const auto t1 = std::chrono::high_resolution_clock::now();

		function();

		const auto t2 = std::chrono::high_resolution_clock::now();
		const std::chrono::duration<double, std::milli> duration = t2 - t1;

		std::cout << std::fixed << std::setprecision(5);
		std::cout << "Computation time: " << duration.count() / 1000.0 << "s\n\n";
	};

	search(searchA);
	search(searchB);
	search(searchC);
}