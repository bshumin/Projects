/* this uses log() from the math library, which might need the library */
/* linked in with -lm on the compile command line */

/**************************************************************************/

/* These functions are for use in the simulator */
#ifndef randsim_h
#define randsim_h 1
double interarrival_time(void);

double enter_airline_queue_time(void);

#endif
