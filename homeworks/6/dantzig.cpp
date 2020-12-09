#include <array>
#include <iomanip>
#include <iostream>
#include <limits>

constexpr long ca = 20;
constexpr long cb = 30;

constexpr std::size_t dimensions = 2;

std::array<std::array<double, 6>, 4> matrix
{{
	{ 1.0, -20.0, -30.0, 0.0, 0.0, 0.0 },
	{ 0.0,   1.0,   3.0, 1.0, 0.0, 0.0 },
	{ 0.0,   3.0,   1.0, 0.0, 1.0, 0.0 },
	{ 0.0,   1.0,   6.0, 0.0, 0.0, 1.0 },
}};

std::array<double, 4> array = { 0.0, 75.0, 99.0, 288.0 };

long f(long a, long b)
{
	return ca * a + cb * b;
}

bool valid(long a, long b)
{
	return a + 3 * b <= 75 && 3 * a + b <= 99 && a + 6 * b <= 288;
}

bool finished(std::size_t& index)
{
	bool finished = true;

	for (index = 1; index <= dimensions; index++)
	{
		if (matrix[0][index] < 0.0) return false;
	}

	return finished;
}

void print()
{
	for (std::size_t i = 0; i < matrix.size(); i++)
	{
		for (std::size_t j = 0; j < matrix[0].size(); j++)
		{
			if (j > 0) std::cout << std::setw(6);
			std::cout << std::fixed << std::setprecision(3) << matrix[i][j] << ' ';
		}
		std::cout << std::fixed << std::setprecision(0) << array[i] << '\n';
	}

	std::cout << '\n';
}

int main(int argc, const char* argv[])
{
	std::size_t index;

	while (!finished(index))
	{
		double min = std::numeric_limits<double>::max();
		std::size_t minIndex = 1;

		for (std::size_t i = 1; i < array.size(); i++)
		{
			if (matrix[i][index] > 0.0 && array[i] / matrix[i][index] < min)
			{
				min = array[i] / matrix[i][index];
				minIndex = i;
			}
		}

		for (std::size_t i = 0; i < matrix.size(); i++)
		{
			if (i != minIndex && matrix[i][index] != 0.0)
			{
				double c = -matrix[i][index] / matrix[minIndex][index];

				for (std::size_t j = 0; j < matrix[0].size(); j++)
				{
					matrix[i][j] += c * matrix[minIndex][j];
				}

				array[i] += c * array[minIndex];
			}
		}
	}

	print();

	const double a = array[2] / matrix[2][1];
	const double b = array[1] / matrix[1][2];
	const double cost = array[0];

	long la = a;
	long lb = b;
	constexpr long window = 5;

	const long maxa = la + window;
	const long maxb = lb + window;

	long maxCost = std::numeric_limits<long>::min();

	for (long i = la - window; i < maxa; i++)
	{
		for (long j = lb - window; j < maxb; j++)
		{
			if (!valid(i, j)) continue;

			long cost = f(i, j);

			if (cost > maxCost)
			{
				la = i;
				lb = j;
				maxCost = cost;
			}
		}
	}

	std::cout << "A = " << la << '\n';
	std::cout << "B = " << lb << '\n';
	std::cout << "Cost = " << maxCost << '\n';

	return 0;
}