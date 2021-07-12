
/* randsim.h */

#ifndef RANDSIM_H
#define RANDSIM_H

/* this uses log() from the math library, which might need the library */
/* linked in with -lm on the compile command line */

/**************************************************************************/

void randsim_init();

void randsim_finalize();

/* Uniform Distribution */
double randsim_uni();

/* Exponential Distribution */
double randsim_exp(double mean);

/* Gaussian Distribution */
double randsim_gauss(double mean, double std);

#endif
