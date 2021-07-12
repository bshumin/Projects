/* event.h */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "sim.h"
#include "time.h"
#include "event.h"  /* must come before priority.h */
#include "priority.h"

static priority_t *p_queue = NULL;

/* initializes events, creates a priority queue
   including the size of the queue */
void event_init(int size)
{
    p_queue = priority_init(size);
}

/* frees up all event space, including space in the priority
   queue */
void event_finalize()
{
    priority_finalize(p_queue);
}

/* allocate a fresh event with empty fields */
event_t *event_create()
{
    event_t *ev;
    ev = (event_t *)malloc(sizeof(struct event_s));
    if (ev)
    {
        memset(ev, 0, sizeof(struct event_s));
    }
    return ev; // NULL on error
}

/* free an event */
void event_destroy(event_t *ev)
{
    free(ev);
}

/* insert the event into the priority queue.  The key
   value is the current sim time plus the event_time in
   event.  Update the event time to the key value. */
void event_schedule(event_t *ev)
{
    ev->event_time += time_get();
    priority_insert(p_queue, ev);
}

/* returns 0 if there are more events in the event queue, and non zero
   if there are no more events. */
int event_empty()
{
    return priority_empty(p_queue);
}

/* remove the next event from the priority queue, set the time to the event’s time, and then return it to the program for execution */
event_t *event_cause()
{
    event_t *ev = (event_t *)priority_remove(p_queue);
    time_set(ev->event_time);
    return ev;  // NULL on error
}
