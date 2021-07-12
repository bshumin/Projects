/* priority.h */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "event.h"
#include "priority.h"

#define PRISIZE 100

/* the priority queue type */
typedef struct priority_s priority_t;
struct priority_s
{
    int size;
    int count;
    event_t *A[PRISIZE];
};

/* create and initialize a new priority queue
   must be able to hold at least size items
   return pointer to the new priority queue, NULL if error */
priority_t *priority_init(int size)
{
    priority_t *pri = (priority_t *)malloc(sizeof(priority_t));
    memset(pri, 0, sizeof(priority_t));
    pri->size = PRISIZE;
    pri->size = size;
    pri->count = 0;
    return pri;
}

void priority_insert(priority_t *pri, event_t *pd)
{
    int child, parent;

    if (priority_full(pri))
    {
        return;
    }

    //printf("PRI INSERT %f\n", pd->event_time);
    pri->count++;
    child = pri->count;
    parent = child/2;

    while(parent != 0)
    {
        if(pd->event_time >= pri->A[parent]->event_time)
        {
            pri->A[child] = pd;
            return;
        }
        else
        {
            pri->A[child] = pri->A[parent];
            child = parent;
            parent = parent/2;
        }
    }
    pri->A[child] = pd;
}

/* remove the highest priority item from the queue
   and return it, return NULL if there is an error */
event_t *priority_remove(priority_t *pri)
{
    int current = 1;
    int child = 2;    /* 2 times current */

    if (priority_empty(pri))
    {
        /* error - empty */
        return NULL;
    }

    event_t *val = pri->A[1];
    event_t *place = pri->A[pri->count];
    pri->count--;

    while(child <= pri->count)
    {
        if (child < pri->count)
        {
            if (pri->A[child + 1]->event_time <
                pri->A[child]->event_time)
            {
                child++;
            }
        }
        if (pri->A[child]->event_time >= place->event_time)
        {
            pri->A[current] = place;
            return val;
        }
        else
        {
            pri->A[current] = pri->A[child];
            current = child;
            child = 2 * current;
        }
    }

    pri->A[current] = place;
    // pri->A[pri->count + 1] = NULL;
    // printf("PRI RETURNS %f\n", val->event_time);
    return val;
}

/* return non-zero if the priority queue us empty */
int priority_empty(priority_t *pri)
{
    return (pri->count == 0);
}

/* return nono-zero if the priority queue is full
   This may be trivial using a linked implementation */
int priority_full(priority_t *pri)
{
    return (pri->count == pri->size);
}

/* free all resourced used by the priority queue then free
   the queue itself */
void priority_finalize(priority_t *pri)
{
    free(pri);
}
