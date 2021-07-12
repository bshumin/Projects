/* FIFO queues */
/*
  Seth Kantz
  Brandon Shumin
  Bradly Sanders
  queue.c
   - function definitions and stuff

  TODO:
  - test peek, size, and full functions
  - test functions with new typedefs
  - update queue dType
 */

#ifndef Q_C
#define Q_C 1

#include "defs.h"

/*
  initializes and returns a pointer to a new queue
  INPUT: size of queue to be made (as an int)
  OUTPUT:  ptr to queue_t
*/
queue_t * queue_init(int size){

  queue_t *newQ = (queue_t *)malloc(sizeof(queue_t));
  newQ->head = NULL;
  newQ->tail = NULL;
  newQ->max = size;
  return newQ;
}

/*
  Adds qData d to a new node at the tail of the queue
  INPUT: queue as ptr,  data as int
  OUTPUT: void
*/
void queue_insert(queue_t *qPtr, int d){
  queue_t *q = qPtr;
  qNode *new = (qNode *)malloc(sizeof(qNode *));
  qNode *temp;
  if(q !=NULL && new != NULL){
    // if q exists and space for new exists
    if(q->head == NULL){
      // if empty list
      new->data = d;
      new->next = NULL;
      new->prev = NULL;
      q->head = new;
      q->tail = new;
    } else {
      temp = q->tail;
      new->data = d;
      new->next = NULL;
      new->prev = temp;
      temp->next = new;
      q->tail = new;
    }
  }
}

/*
  INPUT:  pointer to queue_t
  OUTPUT:  ptr to node
  - find next item in list but dont remove it
*/
qNode * queue_peek(queue_t *qPtr){
  // if not empty
  queue_t *q = qPtr;
  if(q != NULL){
    // if not empty
    if(q->head != NULL)
      return q->head;
  }
  else{
    // if couldnt find item
    return NULL;
  }
  return NULL;
}

/*
  removes the first node and sets the qData pointer to the
  value removed
  INPUT: ptr to queue_t, int * that will be set to removed data
*/
void queue_remove(queue_t *qPtr, int *d){
  queue_t *q = qPtr;
  qNode *temp;
  if(q != NULL){

    // if not empty
    if(q->head != NULL){
      temp = q->head;
      // if one element
      if(temp->next == NULL){
	q->head = NULL;
	q->tail = NULL;
      }else{
	// if multiple elements
	q->head = temp->next;
	q->head->prev = NULL;
      }

      *d = temp->data;
      free(temp);
    }
  }
}

/*
  returns size of queue
  INPUT:     ptr to queue_t
  OUTPUT:    size as int

*/
int queue_size(queue_t *qPtr){
  queue_t *q = qPtr;
  qNode *rov;
  int count = 0;

  //check if queue is not equal
  if(q==NULL) {
    return 0;
  } else {
    rov = q->head;
    while(rov != NULL){
      count++;
      rov = rov->next;
    }
    return count;
  }

}

/* ****MIGHT NEED TO CHANGE******
  - Checks to see if queue is full
  INPUT:     queue_t
  OUTPUT:    returns 0 always since linked list
*/
int queue_full(queue_t *qPtr){
  return 0;
}

/*
  frees each nodes data
  frees said node
  frees queue
  INPUT: *queue_t
*/
void queue_finalize(queue_t *qPtr){
  qNode *rov, *next;
  if(qPtr!=NULL){
    rov = qPtr->head;
    while(rov != NULL){
      next = rov->next;
      free(rov);
      rov = next;
    }
    free(qPtr);
  }
  qPtr = NULL;
}



#endif
