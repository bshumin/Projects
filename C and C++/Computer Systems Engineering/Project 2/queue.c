/* queue.h */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "sim.h"
#include "queue.h"

typedef struct link_s link_t;
struct link_s
{
    q_data_t *data;
    link_t *link;
};

struct queue_s
{
    int numitems;
    link_t *head;  /* remove here */
    link_t *tail;  /* insert here */
};

/* create and initialize a new queue 
   must be able to hold at least size items
   return pointer to the new queue, NULL if error */
queue_t *queue_init(int size)
{
    queue_t *q = (queue_t *)malloc(sizeof(queue_t));
    memset(q, 0, sizeof(queue_t));
    return q;
}

void queue_insert(queue_t *q, q_data_t *dat)
{
    if (q->head == NULL && q->tail == NULL)
    {
        q->head = (link_t *)malloc(sizeof(link_t));
        q->tail = q->head;
    }
    else
    {
        q->tail->link = (link_t *)malloc(sizeof(link_t));
        q->tail = q->tail->link;
    }
    q->tail->link = NULL;
    q->tail->data = dat;
    q->numitems++;
}

/* return the next item in the queue but do not remove it
   return NULL if the queue is empty or on error */
q_data_t *queue_peek(queue_t *q)
{
    if (q->numitems == 0 || q->head == NULL || q->tail == NULL)
    {
        return NULL;
    }
    else
    {
        return q->head->data;
    }
}

/* remove the next item from the queue 
   and return it, return NULL if there is an error */
q_data_t *queue_remove(queue_t *q)
{
    link_t *next;
    if (q->numitems == 0 || q->head == NULL || q->tail == NULL)
    {
        return NULL;
    }
    else
    {
        next = q->head;
        q_data_t *next_d;
        if (q->tail == q->head)
        {
            q->tail = q->head = NULL;
        }
	if(q->head!=NULL)
	  q->head = q->head->link;
        q->numitems--;
        next_d = next->data;
        free(next);
        return next_d;
    }
}
/* return the number of items in the queue 
   You can see if queue is empty with this */
int queue_size(queue_t *q)
{
    return q->numitems;
}

/* return nono-zero if the queue is full
   This may be trivial using a linked implementation */
int queue_full(queue_t *q)
{
    return (0);
}

/* free all resourced used by the queue then free
   the queue itself */
void queue_finalize(queue_t *q)
{
    link_t *rover = q->head;
    link_t *item = rover;
    while(rover)
    {
        item = rover;
        rover = rover->link;
        free(item);
    }
    free(q);
}

