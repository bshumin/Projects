/* time.h */
#ifndef time_h
#define time_h 1

/* creates a double for using as the global sim time */
void time_init(void);

/* cleans up the time system as needed */
void time_fini(void);

/* returns the current sim time */
double time_get(void);

/* sets the current sim time */
void time_set(double t);

/* these return a random time which is used to set the
   time each passenger takes to make each of these steps
   these times are set into the corresponding event
   before it is scheduled. */
double time_arrive(void);

double time_airline(void);

double time_id(void);

double time_scan(void);

double time_train(void);

double time_gate(void);

#endif
