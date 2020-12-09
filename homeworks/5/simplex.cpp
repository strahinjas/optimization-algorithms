#include <cmath>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <limits>

#define NR_END 1
#define FREE_ARG char*
#define TINY 1.0e-10 // A small number.
#define NMAX 5000	 // Maximum allowed number of function evaluations.
#define GET_PSUM                               \
	for (j = 1; j <= ndim; j++)                \
	{                                          \
		for (sum = 0.0, i = 1; i <= mpts; i++) \
			sum += p[i][j];                    \
		psum[j] = sum;                         \
	}
#define SWAP(a, b)  \
	{               \
		swap = (a); \
		(a) = (b);  \
		(b) = swap; \
	}

#define N 2
#define VARIABLE_E

using namespace std;

void nrerror(const char error_text[])
/* Numerical Recipes standard error handler */
{
	fprintf(stderr, "Numerical Recipes run-time error...\n");
	fprintf(stderr, "%s\n", error_text);
	fprintf(stderr, "...now exiting to system...\n");
	exit(1);
}

double* vector(long nl, long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
	double* v;

	v = (double*) malloc((size_t) ((nh - nl + 1 + NR_END) * sizeof(double)));
	if (!v)
		nrerror("allocation failure in vector()");
	return v - nl + NR_END;
}

void free_vector(double* v, long nl, long nh)
/* free a double vector allocated with vector() */
{
	free((FREE_ARG) (v + nl - NR_END));
}

void NMSimplex(double** p, double y[], int ndim, double ftol, double (*funk)(double[]), int* nfunk)
/* Multidimensional minimization of the function funk(x) 
   where x[1..ndim] is a vector in ndim dimensions, 
   by the downhill simplex method of Nelder and Mead.
   The matrix p[1..ndim + 1][1..ndim] is input.
   Its ndim + 1 rows are ndim - dimensional vectors 
   which are the vertices of the starting simplex.
   Also input is the vector y[1..ndim + 1], 
   whose components must be preinitialized to the values of funk 
   evaluated at the ndim + 1 vertices(rows) of p; 
   and ftol the fractional convergence tolerance to be achieved in the function value(n.b.!).
   On output, p and y will have been reset to ndim + 1 new points 
   all within ftol of a minimum function value, 
   and nfunk gives the number of function evaluations taken. */
{
	double amotry(double** p, double y[], double psum[], int ndim, double (*funk)(double[]), int ihi, double fac);
	int i, ihi, ilo, inhi, j, mpts = ndim + 1;
	double rtol, sum, swap, ysave, ytry, *psum;

	psum = vector(1, ndim);
	*nfunk = 0;
	GET_PSUM
	while (true)
	{
		ilo = 1;
		// First we must determine which point is the highest(worst), next - highest, and lowest(best), by looping over the points in the simplex.
		ihi = y[1] > y[2] ? (inhi = 2, 1) : (inhi = 1, 2);
		for (i = 1; i <= mpts; i++)
		{
			if (y[i] <= y[ilo])
				ilo = i;
			if (y[i] > y[ihi])
			{
				inhi = ihi;
				ihi = i;
			}
			else if (y[i] > y[inhi] && i != ihi)
				inhi = i;
		}
		rtol = (double) (2.0 * fabs(y[ihi] - y[ilo]) / (fabs(y[ihi]) + fabs(y[ilo]) + TINY));
		// Compute the fractional range from highest to lowest and return if satisfactory.
		if (rtol < ftol)
		{
			// If returning, put best point and value in slot 1.
			SWAP(y[1], y[ilo])
			for (i = 1; i <= ndim; i++)
				SWAP(p[1][i], p[ilo][i])
				break;
		}
		// if (*nfunk >= NMAX) nrerror("NMAX exceeded");
		if (*nfunk >= NMAX) return;
		*nfunk += 2;
		// Begin a new iteration.First extrapolate by a factor −1 through the face of the simplex across from the high point, i.e., reﬂect the simplex from the high point.
		ytry = amotry(p, y, psum, ndim, funk, ihi, -1.0);
		if (ytry <= y[ilo])
			// Gives a result better than the best point, so try an additional extrapolation by a factor 2.
			ytry = amotry(p, y, psum, ndim, funk, ihi, 2.0);
		else if (ytry >= y[inhi])
		{
			// The reﬂected point is worse than the second - highest, so look for an intermediate lower point, i.e., do a one - dimensional contraction.
			ysave = y[ihi];
			ytry = amotry(p, y, psum, ndim, funk, ihi, 0.5);
			if (ytry >= ysave)
			{
				// Can’t seem to get rid of that high point.Better contract around the lowest(best) point.
				for (i = 1; i <= mpts; i++)
				{
					if (i != ilo)
					{
						for (j = 1; j <= ndim; j++)
							p[i][j] = psum[j] = (double) (0.5 * (p[i][j] + p[ilo][j]));
						y[i] = (*funk)(psum);
					}
				}
				*nfunk += ndim; // Keep track of function evaluations.
				GET_PSUM		// Recompute psum.
			}
		}
		else
			--(*nfunk); // Correct the evaluation count.
	}					// Go back for the test of doneness and the next iteration.
	free_vector(psum, 1, ndim);
}

double amotry(double** p, double y[], double psum[], int ndim, double (*funk)(double[]), int ihi, double fac)
// Extrapolates by a factor fac through the face of the simplex across from the high point, tries it, and replaces the high point if the new point is better.
{
	int j;
	double fac1, fac2, ytry, *ptry;
	ptry = vector(1, ndim);
	fac1 = (double) ((1.0 - fac) / ndim);
	fac2 = fac1 - fac;
	for (j = 1; j <= ndim; j++)
		ptry[j] = psum[j] * fac1 - p[ihi][j] * fac2;
	ytry = (*funk)(ptry); // Evaluate the function at the trial point.

	if (ytry < y[ihi])
	{
		// If it’s better than the highest, then replace the highest.
		y[ihi] = ytry;
		for (j = 1; j <= ndim; j++)
		{
			psum[j] += ptry[j] - p[ihi][j];
			p[ihi][j] = ptry[j];
		}
	}
	free_vector(ptry, 1, ndim);
	return ytry;
}

#define Rc  1000.0
#define Re  1000.0
#define Vcc 12.0
#define Is  0.1e-12
#define Vt  0.026
#define Bf  100.0
#define Br  2.0

double E = 5.0;

inline double getIb(double Vbe, double Vbc)
{
	return Is * ((exp(Vbe / Vt) - 1) / Bf + (exp(Vbc / Vt) - 1) / Br);
}

inline double getIc(double Vbe, double Vbc)
{
	return Is * ((exp(Vbe / Vt)) - (exp(Vbc / Vt)) - (exp(Vbc / Vt) - 1) / Br);
}

// optimization function
double f(double x[])
{
	const double Vbe = x[1];
	const double Vbc = x[2];

	const double Ib = getIb(Vbe, Vbc);
	const double Ic = getIc(Vbe, Vbc);

	if (Ib < 0 || Ic < 0) return numeric_limits<double>::max();

	const double Ie = Ib + Ic;

	return abs(E - Vbe - Re * Ie) + abs(Vcc - Rc * Ic + Vbc - E);
}

int main()
{
	// memory allocation
	double** p = new double *[N + 2];
	for (int i = 0; i < N + 2; i++)
		p[i] = new double[N + 1];
	double* y = new double[N + 2];
	double ftol = (double)1e-10;
	int nfunk;

#ifdef VARIABLE_E
	ofstream vbe("vbe.txt", ofstream::trunc);
	ofstream ic("ic.txt", ofstream::trunc);
	ofstream vout("vout.txt", ofstream::trunc);

	for (double e = 0.0; e <= 10.0; e += 0.1)
	{
		E = e;

		// initial simplex
		for (int i = 0; i < N + 2; i++)
		{
			for (int j = 0; j < N + 1; j++)
			{
				p[i][j] = 0.0;
				if (i > 1 && j == i - 1)
					p[i][i - 1] = 1.0;
			}
			y[i] = f(p[i]);
		}

		// optimization
		NMSimplex(p, y, N, ftol, f, &nfunk);

		const double Vbe = p[1][1];
		const double Vbc = p[1][2];

		const double Ib = getIb(Vbe, Vbc);
		const double Ic = getIc(Vbe, Vbc);

		vbe  << E << ' ' << Vbe << '\n';
		ic   << E << ' ' << Ic  << '\n';
		vout << E << ' ' << (Re * (Ib + Ic) + Vbe - Vbc) << '\n';

		// output
		printf("E = %4.1lfV => Vbe = %9.5lfV, Vbc = %9.5lfV, f(xmin) = %2.5lf\n", E, Vbe, Vbc, y[1]);
	}
#else
	// initial simplex
	for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 1; j++)
		{
			p[i][j] = 0.0;
			if (i > 1 && j == i - 1)
				p[i][i - 1] = 1.0;
		}
		y[i] = f(p[i]);
	}

	// optimization
	NMSimplex(p, y, N, ftol, f, &nfunk);

	const double Vbe = p[1][1];
	const double Vbc = p[1][2];

	// output
	printf("E = %2.1lfV => Vbe = %2.5lfV, Vbc = %2.5lfV, f(xmin) = %2.5lf\n", E, Vbe, Vbc, y[1]);
#endif

	// clean-up
	for (int i = 0; i < N; i++)
		delete[] p[i];
	delete[] p;

#ifdef VARIABLE_E
	vbe.close();
	ic.close();
	vout.close();
#endif

	return 0;
}