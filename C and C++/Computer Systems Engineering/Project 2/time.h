/* time.h */

#ifndef TIME_H
#define TIME_H

/* creates a double for using as the global sim time 
   this also initializes the randsim module */
void time_init();

/* cleans up the time system as needed */
void time_finalize();

/* returns the current sim time */
double time_get();

/* sets the current sim time */
void time_set(double t);

/* these return a random time which is used to set the
   time each passenger takes to make each of these steps
   these times are set into the corresponding event 
   before it is scheduled. These should be moved from 
   randsim.c to time.c */
double time_arrive();
double time_airlineQ(); // passenger enters airline queue
double time_airline();  // time airline takes to ticket and bag
double time_idQ();      // passenger enters security queue
double time_id();       // time security takes reviewing ID
double time_scanQ();    // passenger enters a scanner queue
double time_scan();     // time it takes to scan passenger
double time_trainQ();   // passenger enters a queue for train
double time_train();    // times for train to get to gate area
double time_gate();     // time for passenger to get to gate

#endif
