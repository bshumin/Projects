#include<stdio.h>
#include<stdlib.h>

// Link List structure declaration
struct linkList{
	int size;
	struct linkList *link;
} lnode;

// Insert 1st 10 List item at the specified area of the linked list
void InsertItemHead(struct linkList *L, int *count, int lowBound, int upBound);

// Insert 2nd 10 List item at the specified area of the linked list
void InsertItemTail(struct linkList *L, int *count, int lowBound, int upBound);

// Insert 3rd 10 List item at the specified area of the linked list
void InsertItemMid(struct linkList *L, int *count, int lowBound, int upBound);

// Remove List item(s) at the head of the linked list
struct linkList* RemoveItemHead(struct linkList *L, int *count, int lowBound,
	int upBound);

// Remove List item(s) at the tail of the linked list
struct linkList* RemoveItemTail(struct linkList *L, int *count, int lowBound,
	int upBound);

// Print out all Linked List items
void PrintItemList(struct linkList *L, int *count);

int main(void){
	struct linkList *L; //Pointer to head of the link list
	L = (struct linkList *) malloc(sizeof(struct linkList));
	int count = 0;

	// Insert 1-10 at head of linked list
	InsertItemHead(L, &count, 1, 10);

	// Insert 21-30 at tail of linked list
	InsertItemTail(L, &count, 21, 30);

	// Insert 11-20 in middle of linked list
	InsertItemMid(L, &count, 11, 20);

	// Remove 5 items at tail of the list
	L = RemoveItemHead(L, &count, 0, 5);

	// Remove 5 items at tail of the list
	L = RemoveItemTail(L, &count, 0, 5);

	// print out remaining linked list nodes
	PrintItemList(L, &count);

	return 0;
}

// Insert 1st 10 List item at the head of the list
void InsertItemHead(struct linkList *L, int *count, int lowBound, int upBound) {
	int i;
	struct linkList *Rover;
	Rover = L;

	// Enter values in the head of the linked list
	for(i=lowBound;i<=upBound;i++) {
		Rover->size = i;
		(*count)++;

		// Link tail to head if end of loop if all values have been inserted
		if(i == upBound) {
			Rover->link = L;
		} else{
			Rover->link = (struct linkList *) malloc(sizeof(struct linkList));
			Rover = Rover->link;
		}
	}
}

// Insert 2nd 10 List item at the tail of the list
void InsertItemTail(struct linkList *L, int *count, int lowBound, int upBound) {
	int i;
	struct linkList *Rover;

	// Start Rover at head of linked list
	Rover = L;

	// Find tail of the list
	while(Rover->link != L) {
		Rover = Rover->link;
	}

	// Create new node for data input
	Rover->link = (struct linkList *) malloc(sizeof(struct linkList));
	Rover = Rover->link;

	// Insert nodes at the tail of the list
	for(i=lowBound;i<=upBound;i++) {
		(*count)++; //keep track of node count
		Rover->size = i;
		// if all values have been inserted, re-connect links in the linked list
		if(i == upBound) {
			Rover->link = L;
		} else{
			Rover->link = (struct linkList *) malloc(sizeof(struct linkList));
			Rover = Rover->link;
		}
	}
}

// Insert 3rd 10 List item at the middle of the list
void InsertItemMid(struct linkList *L, int *count, int lowBound, int upBound) {
	int i;
	struct linkList *Rover, *RoverTwo;
	Rover = L;

	//Find midpoint of the List
	for(i=0;i<((*count)/2)-1;i++) {
		Rover = Rover->link;
	}

	// Save position at the end of the middle split
	RoverTwo = Rover->link;

	// Create new node for data  entry
	Rover->link = (struct linkList *) malloc(sizeof(struct linkList));
	Rover = Rover->link;

	// insert nodes in the middle of the linked list
	for(i=lowBound;i<=upBound;i++) {
		(*count)++;
		Rover->size = i;
		// If all values have been inserted, re-connect links in the linked list
		if(i == upBound) {
			Rover->size = i;
			Rover->link = RoverTwo;
		} else{
			Rover->link = (struct linkList *) malloc(sizeof(struct linkList));
			Rover = Rover->link;
		}
	}
}

// Remove List item at the head of the linked list
struct linkList* RemoveItemHead(struct linkList *L, int *count, int lowBound,
	int upBound){
	int i;
	struct linkList *Rover, *RoverTail, *RoverHead;

	// Find tail node of linked list
	Rover = L;
	do{
		Rover = Rover->link;
	}while(Rover->link != L);

	RoverTail = Rover; // RoverTail holds the tail of the linked list
	Rover  = L;

	// Find and remove the first 5 nodes in the linked list using free()
	for(i=lowBound;i<upBound;i++) {
		(*count)--;
		RoverHead = Rover->link;
		free(Rover);
		Rover = RoverHead;
	}

	//Return the new head of the linked list
	RoverTail->link = RoverHead;
	return RoverHead;
}

// Remove List item at the tail of the linked list
struct linkList* RemoveItemTail(struct linkList *L, int *count, int lowBound,
	int upBound){
	int i;
	struct linkList *Rover, *RoverTail;

	// find tail node of linked list
	Rover = L;
	do{
		Rover = Rover->link;
	} while(Rover->link != L);

	// RoverTail holds the tail of the linked list
	RoverTail = Rover;

	// Find and remove last 5 nodes in the linked list using free()
	for(i=lowBound;i<upBound;i++) {
		(*count)--;
		Rover = L;
		while(Rover->link != L){
			RoverTail = Rover;
			Rover = Rover->link;
		}
		free(Rover);
		RoverTail->link = L;
	}

	return L;
}

//Print out all Linked List items
void PrintItemList(struct linkList *L, int *count){
	struct linkList *Rover;
	int i = 1;
	Rover = L;

	//print out node data
	fprintf(stdout,"---------------------------------");
	do {
		fprintf(stdout,"\nNode = %d\nValue = %d\n", i, Rover->size);
		fprintf(stdout,"Address = 0x%p\nNext Address = 0x%p\n", Rover, Rover->link);
		fprintf(stdout,"---------------------------------");

		//proceed to next node
		Rover = Rover->link;
		i++;
	}while(Rover != L);

	//print out node count
	fprintf(stdout,"\nNode Count: %d\n\n", (*count));
}
