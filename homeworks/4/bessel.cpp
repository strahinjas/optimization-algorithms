#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>

constexpr int precision  = 12;
constexpr double epsilon = 1e-12;

double bisectionMethod(unsigned n, double a, double b)
{
	if (a > b) return 0.0;

	double fa = std::sph_bessel(n, a);
	double fb = std::sph_bessel(n, b);

	while (true)
	{
		double x  = 0.5 * (a + b);
		double fx = std::sph_bessel(n, x);

		if (std::abs(fx) <= epsilon) return x;

		if (fa * fx < 0.0)
		{
			b  = x;
			fb = fx;
		}
		else
		{
			a  = x;
			fa = fx;
		}
	}
}

void findRoots(unsigned n, const std::array<double, 3>& points)
{
	double x1 = bisectionMethod(n, points[0], points[1]);
	double x2 = bisectionMethod(n, points[1], points[2]);

	std::cout << std::fixed << std::setprecision(precision);
	std::cout << "n = " << n << " => x1 = " << x1 << ", x2 = " << x2 << '\n';
}

int main()
{
	constexpr std::array<double, 3> points1 = { 2.0, 6.0, 10.0 };
	constexpr std::array<double, 3> points2 = { 4.0, 8.0, 12.0 };

	findRoots(1, points1);
	findRoots(2, points2);

	return 0;
}