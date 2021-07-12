
/* this uses log() from the math library, which might need the library */
/* linked in with -lm on the compile command line */

/* NOTE TO PROGRAMMERS - you MUST put a function prototype in your code */
/* for each of the functions you plan to call. */
#include "defs.h"
/* not used in this version, values must be passed in */
#if 0
/* double M1 = 4.5;   median of the exponential function */
/* double M2 = 4.5;   median of the gaussian function */
/* double D2 = 2.2;   Std deviation of the gaussian function */
#endif

/* Uniform Distribution */
//static double randsim_uni();

/* Exponential Distribution */
static double randsim_exp(double mean);

/* Gaussian Distribution */
static double randsim_gauss(double mean, double std);

/* Uniform Distribution */
static double randsim_uni()
{
	long int U = 0;
	while (U == 0)
		U = random();
	return ((double)U / (double)RAND_MAX);
}

/* Exponential Distribution */
static double randsim_exp(double mean)
{
	return -mean * log(randsim_uni());
}

static double rand_gas_full(double mean, double std)
{
	static int iset = 0;
	static double gset;
	double fac, rsq, v1, v2;
	if (iset == 0)
        {
		do {
			v1 = 2.0 * randsim_uni() - 1.0;
			v2 = 2.0 * randsim_uni() - 1.0;
			rsq = v1 * v1 + v2 * v2;
		} while (rsq >= 1.0 || rsq == 0.0);
		fac = sqrt(-2.0 * log(rsq) / rsq);
		gset = v1 * fac * std;
		iset = 1;
		return (v2 * fac * std) + mean;
	} else {
		iset = 0;
		return gset + mean;
	}
}

/* Gaussian Distribution - prevents values outside of 2 STD DEVs */
double randsim_gauss(double mean, double std)
{
	double u = -1.0;
	while((u < 0.0) || (u > (2.0 * mean)))
	{
		u = rand_gas_full(mean, std);
	}
	return u;
}

/******************************************************************/

/* Two times for Phase I of program */

double interarrival_time(void)
{
	return randsim_exp(2.0);
}

double enter_airline_queue_time(void)
{
	return randsim_gauss(60, 20);
}
