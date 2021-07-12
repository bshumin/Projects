/*Brandon Shumin
  ECE 2230 Section 001
  Oct 9, 2019
  File: queue.c
  File Description: file containing all the function definitions used by
  queuetest.c for use on the queue defined in queuetest.c.
*/
#include<stdio.h>
#include<stdlib.h>
#include "queue.h"

//Function for initializing queue
qbase * Queue_Init(qptr Queue){
  struct qbase *hdr;
  hdr = (struct qbase *)malloc(sizeof(struct qbase));

  //
  hdr->head = NULL;
  hdr->tail = NULL;
  return hdr;
}

// FUnction for inserting a specified value into the queue at tail
qbase * Queue_Insert(qptr Queue, qdata value){
  struct qbase *hdr = (struct qbase *)Queue;
  struct qnode *new = (struct qnode *)malloc(sizeof(struct qnode));

  //check if queue pointer is NULL
  if(hdr == NULL){
    return hdr;
  }

  struct qnode *rover = hdr->tail;

  //set data value for new node
  new->data = value;
  new->next = NULL;

  //check if no item in queue
  if(hdr->head == NULL && hdr->tail == NULL) {
    hdr->head = new;
    hdr->tail = new;

    return hdr;
  }

  // otherwise insert at tail of queue and redefine tail of hdr
  rover = hdr->tail;
  rover->next = new;
  hdr->tail = new;

  return hdr;
}

//Function to remove node from the head of the queue
qbase * Queue_Remove(qptr Queue){
  struct qbase *hdr = (struct qbase *)Queue;
  struct qnode *rover;

  //if queue pointer is NULL, return NULL pointer (hdr)
  if(hdr == NULL) {
    return hdr;
  }

  //check for empty queue, return empty queue if so
  if(hdr->tail==NULL && hdr->head==NULL) {
    return hdr;
  }

  //else, remove head node, while adjusting the new head of the queue
  rover = hdr->head;
  hdr->head = rover->next;
  free(rover);
  return hdr;
}

//free all nodes in the queue and free the queue pointer
qbase * Queue_Destroy(qptr Queue){
  struct qbase *hdr = (struct qbase *)Queue;
  struct qnode *rover = hdr->head;
  struct qnode *destroy;

  //iterate through all nodes and free them
  while(rover){
    destroy = rover;
    rover = rover->next;
    free(destroy);
  }

  //free the queue pointer
  free(hdr);

  return NULL;
}
