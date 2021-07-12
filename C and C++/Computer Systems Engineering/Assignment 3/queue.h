/*Brandon Shumin
  ECE 2230 Section 001
  Oct 9, 2019
  File: queue.h
  File Description: Header file containing the structure declarations, typedefs,
  and function declarations to be used in queue.c and queuetest.c
*/
// create/define abstract data types
//typedef void pointer and int type
typedef int qdata;
typedef void *qptr;

//node structure
typedef struct qnode{
  qdata data;
  struct qnode *next;
} qnode;

//queue base pointer structure
typedef struct qbase{
  struct qnode *head;
  struct qnode *tail;
} qbase;

//prototype the functions to be used in queue.c and queuetest.c
qbase * Queue_Init(qptr Queue);
qbase * Queue_Insert(qptr Queue, qdata value);
qbase * Queue_Remove(qptr Queue);
qbase * Queue_Destroy(qptr Queue);
