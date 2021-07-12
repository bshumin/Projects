/* queue.h */

#ifndef queue_h
#define queue_h 1

typedef int q_dtype;

typedef struct node{
  int data;
  struct node *next;
  struct node *prev;
} qNode;

typedef struct queue_s{
  qNode *head;
  qNode *tail;
  int max;
} queue_t;

/* create and initialize a new queue
   must be able to hold at least size items
   return pointer to the new queue, NULL if error */
queue_t * queue_init(int size);

/* insert an item into the queue
   return 0 if successful, -1 otherwise */
void queue_insert(queue_t *qPtr, int d);

/* return the next item in the queue but do not remove it
   return NULL if the queue is empty or an error */
qNode * queue_peek(queue_t *qPtr);

/* remove the next item from the queue
   and return it, return NULL if there is an error */
void queue_remove(queue_t *qPtr, int *d);

/* return the number of items in the queue
   You can see if queue is empty with this */
int queue_size(queue_t *qPtr);

/* return nono-zero if the queue is full
   This may be trivial using a linked implementation */
int queue_full(queue_t *qPtr);

/* free all resourced used by the queue then free
   the queue itself */
void queue_finalize(queue_t *qPtr);

#endif
