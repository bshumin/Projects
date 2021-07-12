/* event.h */

#ifndef EVENT_H
#define EVENT_H

#include "sim.h"

typedef struct event_s event_t;
struct event_s
{
   int event_type;        /* type of event – see below */
   // queue_t *queue         /* queue passenger is waiting in */
   double event_time;     /* sim time when event occurs */
   passenger_t *passenger;/* passenger related to this event */
};

/* initializes events, creates a priority queue 
   including the size of the queue */
void event_init(int size);

/* frees up all event space, including space in the priority
   queue */
void event_finalize();

/* allocate a fresh event with empty fields */
event_t *event_create();

/* free an event */
void event_destroy(event_t *e);

/* insert the event into the priority queue.  The key
   value is the current sim time plus the event_time in
   event.  Update the event time to the key value. */
void event_schedule(event_t *e); 

/* returns 0 if there are more events in the event queue, and non zero
   if there are no more events. */
int event_empty();

/* remove the next event from the priority queue, set the time to the event’s time, and then return it to the program for execution */
event_t *event_cause();

#endif
