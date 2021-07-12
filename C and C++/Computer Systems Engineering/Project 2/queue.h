/* queue.h */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_s queue_t;
typedef passenger_t q_data_t;

/* create and initialize a new queue 
   must be able to hold at least size items
   return pointer to the new queue, NULL if error */
queue_t *queue_init(int size);

/* insert an item into the queue 
   return 0 if successful, -1 otherwise */
void queue_insert(queue_t *q, q_data_t *c);

/* return the next item in the queue but do not remove it
   return NULL if the queue is empty or on error */
q_data_t *queue_peek(queue_t *q);

/* remove the next item from the queue 
   and return it, return NULL if there is an error */
q_data_t *queue_remove(queue_t *q);

/* return the number of items in the queue 
   You can see if queue is empty with this */
int queue_size(queue_t *q);

/* return nono-zero if the queue is full
   This may be trivial using a linked implementation */
int queue_full(queue_t *q);

/* free all resourced used by the queue then free
   the queue itself */
void queue_finalize(queue_t *q);

#endif
