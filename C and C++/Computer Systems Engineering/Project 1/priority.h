/* priority.h */

#ifndef priority_h
#define priority_h 1

typedef struct Heap_s{
  event_t ** array;
  int items;
  int MAXCAPACITY;
}priority_t;


/* create and initialize a new priority queue
   must be able to hold at least size items
   return pointer to the new priority queue, NULL if error */
priority_t *priority_init(int size);

/* insert an item into the priority queue
   return 0 if successful, -1 otherwise */
int priority_insert(priority_t *heap, event_t *ev);

/* remove the highest priority item from the queue
   and return it, return NULL if there is an error */
event_t *priority_remove(priority_t *heap);

/* return non-zero if the priority queue is empty */
int priority_empty(priority_t *heap);

/* return nono-zero if the priority queue is full
   This may be trivial using a linked implementation */
int priority_full(priority_t *heap);

/* free all resources used by the priority queue then free
   the queue itself */
void priority_finalize(priority_t *heap);

/* heapify the heap with main at top */
priority_t * min_heapify (priority_t *heap, int i);

/* heapify bottom to top */
priority_t * heapify_bottom_to_top(priority_t *h,int i);

/* heapify top to bottom */
priority_t * heapify_top_to_bottom(priority_t *h, int parent);

// initialize a priority queue
priority_t * eq;


#endif
