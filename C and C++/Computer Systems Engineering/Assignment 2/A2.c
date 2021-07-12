#include<stdio.h>
#include<stdlib.h>

//structure used to define queue and stack
struct lnode{
	int value;
	struct lnode *link;
} *stack, *queue;

// add node to the tail of a queue or head of a stack
struct lnode* Push(struct lnode *list, int num);

//pop head node off of a stack
struct lnode* Pop(struct lnode *stack, int *hold);

//pop the head node off of a queue
struct lnode* Dequeue(struct lnode *queue, int *hold);

int main(void){
	int i, hold = 0;
	queue = NULL;
  stack = NULL;

	// push 1-20 to a stack
	for(i=1; i<=20; i++) {
		stack = Push(stack, i);
	}

	//pop off stack values and enqueue those values into the queue
	while(stack != NULL){
		stack = Pop(stack, &hold);
		queue = Push(queue, hold);
	}

	// dequeue and print off values
	fprintf(stdout, "Final Dequeued Values: ");
	while(queue != NULL) {
		queue = Dequeue(queue, &hold);
		fprintf(stdout,"%d ",hold);
	}
	fprintf(stdout,"\n");

	return 0;
}

struct lnode* Push(struct lnode *list, int num){
	struct lnode *Rover;
	Rover = (struct lnode *) malloc(sizeof(struct lnode));

	//insert new node at head of stack
	Rover->value = num;
	Rover->link = list;
	list = Rover;

	return list;
}

struct lnode* Pop(struct lnode *stack, int *hold){
	struct lnode *Rover;
	Rover = stack;

	//pop off top value
	*hold = stack->value;
	stack = stack->link;
	free(Rover);
	return stack;
}

struct lnode* Dequeue(struct lnode *queue, int *hold){
	struct lnode *Rover, *Follower;
	Rover = (struct lnode *) malloc(sizeof(struct lnode));
	Rover = queue;

	//check for case of only 1 node left
	if(Rover->link == NULL){
		*hold = Rover->value;
		queue = Rover->link;
		free(Rover);

	//otherwise remove the head node of the queue
	} else{
		while(Rover->link != NULL){
			Follower = Rover;
			Rover = Rover->link;
		}
		Follower->link = Rover->link;
		*hold = Rover->value;
		free(Rover);
	}

	return queue;
}
