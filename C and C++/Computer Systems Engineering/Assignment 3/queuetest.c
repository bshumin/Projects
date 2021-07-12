/*Brandon Shumin
  ECE 2230 Section 001
  Oct 9, 2019
  File: queuetest.c
  File Description: main driving file for the program. This file calls functions
  defined in queue.c using structures and typedefs from queue.h. This program
  checks various edge cases for the functions used to ensure they work without
  resulting in segmentation faults or unexpected results.
*/
#include<stdio.h>
#include<stdlib.h>
#include "queue.h"

int main(void) {
  qptr Queue = NULL;
  int addNum = 0, i = 0;
  qnode * rover;

  //add numbers 0-200 to the queue
  fprintf(stdout,"Inserting 1-200\n");
  fprintf(stdout,"----------\n");
  Queue = Queue_Init(Queue);
  for(i=0;i<=200;i++){
    Queue = Queue_Insert(Queue,addNum);
    addNum++;
  }

  //print out Queue values
  rover = ((struct qbase *)Queue)->head;
  while(rover!=NULL) {
    printf("%d\n", rover->data);
    rover = rover->next;
  }
  fprintf(stdout,"----------\n\n");

  //remove 50 values from the queue
  fprintf(stdout,"Removing first 50 elements\n");
  fprintf(stdout,"----------\n");
  for(i=0;i<=50;i++){
    Queue = Queue_Remove(Queue);
  }

  //print out values remaining in the queue
  rover = ((struct qbase *)Queue)->head;
  while(rover!=NULL) {
    printf("%d\n", rover->data);
    rover = rover->next;
  }
  fprintf(stdout,"----------\n\n");

  //erase queue and all values it holds
  fprintf(stdout,"----------\n");
  fprintf(stdout,"Clearing Queue\n");
  Queue = Queue_Destroy(Queue);
  fprintf(stdout,"Done\n");
  fprintf(stdout,"----------\n\n");

  //check that queue will not seg fault if pointer is NULL
  fprintf(stdout,"----------\n");
  fprintf(stdout,"Attempting to remove element\n");
  Queue = Queue_Remove(Queue);
  fprintf(stdout,"Success\n");
  fprintf(stdout,"----------\n\n");

  //check #2
  fprintf(stdout,"----------\n");
  fprintf(stdout,"Attempting to insert element\n");
  Queue = Queue_Insert(Queue,1);
  fprintf(stdout,"Success\n");
  fprintf(stdout,"----------\n\n");

  //show that queue can be reinitialized and inserted
  fprintf(stdout,"----------\n");
  fprintf(stdout,"Reinitializing queue\n");
  Queue = Queue_Init(Queue);
  fprintf(stdout,"Attempting to insert value of 1\n");
  Queue = Queue_Insert(Queue,1);

  //print out value of 1
  rover = ((struct qbase *)Queue)->head;
  while(rover!=NULL) {
    printf("%d\n", rover->data);
    rover = rover->next;
  }
  fprintf(stdout,"Success\n");
  fprintf(stdout,"----------\n\n");

  //remove last element of queue
  fprintf(stdout,"----------\n");
  fprintf(stdout,"Removing final element\n");
  Queue = Queue_Remove(Queue);
  fprintf(stdout,"Success\n");
  fprintf(stdout,"----------\n\n");

  //check that printing with no value in queue works
  fprintf(stdout,"----------\n");
  fprintf(stdout,"Attempting to print queue with no elements\n");
  rover = ((struct qbase *)Queue)->head;
  while(rover!=NULL) {
    printf("%d\n", rover->data);
    rover = rover->next;
  }
  fprintf(stdout,"Success\n");
  fprintf(stdout,"----------\n\n");

  //free up memory taken by queue finally
  fprintf(stdout,"----------\n");
  fprintf(stdout,"Freeing queue\n");
  Queue = Queue_Destroy(Queue);
  fprintf(stdout,"Success\n");
  fprintf(stdout,"----------\n\n");

  return 1;
}
