/* Brandon Shumin                                                               //
// Project 1                                                                    //
// ECE 2220                                                                     //
// January 14, 2019                                                             //
//------------------------------------------------------------------------------//
// Program Description: This program asks the user for a number from 1 to 5     //
//                      inclusive, 2 numbers 1 to 10 inclusive a number 10 to   //
//                      12 inclusive, and 2 numbers from 8 to 15 inclusive and  //
//                      then randomly reorders them into an array of the same   //
//                      size. These numbers are added successively and a        //
//                      lottery number is generated from this addition and then //
//                      displayed to the user along with the  original numbers  //
//                      and the final delta sequences. Then, a random lottery   //
//                      number is generated the same way by the program and     //
//                      compared to the user number. This process is repeated   //
//                      one million times, or until the user number matches a   //
//                      randomly generated number.                              //
//------------------------------------------------------------------------------*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(void) {
	
	int deltaNum[6], finalSeq[6] = {0,0,0,0,0,0}, randWin[6] = {0,0,0,0,0,0};
	int i, j, holdVal, randOrder = 0, randPlace, lottoNum[6], totalDelta = 51;
	int k, matchWin = 0;	
	
	//seed the rand() function to create unique outputs each run
	srand(time(NULL));

	// Check to make sure total value of delta nums <= 50
	while (totalDelta > 50) {
		//Initialize deltaNum to allow while loops to runi
		for (i = 0;i < 6;i++) {
			deltaNum[i] = 0;
		}
	

		// User is prompted for values of delta nums and values are
		// checked to confirm they match the desired value parameters
		while (deltaNum[0] > 5 || deltaNum[0] < 1) {
			printf("Please input a number from 1 to 5 inclusive: ");
			scanf("%d", &deltaNum[0]);
			if (deltaNum[0] > 5 || deltaNum[0] < 1) {
				printf("Entered value is outside of the desired range, try again.\n");
			}
		}
		while (deltaNum[1] > 10 || deltaNum[1] < 1) {
			printf("Please input a number from 1 to 10 inclusive: ");
			scanf("%d", &deltaNum[1]);
			if (deltaNum[1] > 10 || deltaNum[1] < 1) {
				printf("Entered value is outside of the desired range, try again.\n");
			}
		}
		while (deltaNum[2] > 10 || deltaNum[2] < 1) {
			printf("Please input another number from 1 to 10 inclusive: ");
			scanf("%d", &deltaNum[2]);
			if (deltaNum[2] > 10 || deltaNum[2] < 1) {
				printf("Entered value is outside of the desired range, try again.\n");
			}
		}
		while (deltaNum[3] > 12 || deltaNum[3] < 10) {
			printf("Please input a number from 10 to 12 inclusive: ");
			scanf("%d", &deltaNum[3]);
			if (deltaNum[3] > 12 || deltaNum[3] < 10) {
				printf("Entered value is outside of the desired range, try again.\n");
			}
		}
		while (deltaNum[4] > 15 || deltaNum[4]  < 8) {
			printf("Please input a number from 8 to 15 inclusive: ");
			scanf("%d", &deltaNum[4]);
			if (deltaNum[4] > 15 || deltaNum[4] < 8) {
				printf("Entered value is outside of the desired range, try again.\n");
			}
		}
		while (deltaNum[5] > 15 || deltaNum[5] < 8) {
			printf("Please input another number from 8 to 15 inclusive: ");
			scanf("%d", &deltaNum[5]);
			if (deltaNum[5] > 15 || deltaNum[5] < 8) {
				printf("Entered value is outside of the desired range, try again.\n");
			}
		}
		
		totalDelta = deltaNum[0] + deltaNum[1] + deltaNum[2]+ deltaNum[3] +
			deltaNum[4] + deltaNum[5];
		
		if (totalDelta > 50) {
			printf("Entered values sum to be more than 50, try again.\n");
		}
	}
	// Order the user delta numbers ascendingly
	for(i = 0;i < 6;i++) {
		for(j = i + 1;j < 6;j++) {
			if(deltaNum[i] > deltaNum[j]) {
				holdVal     = deltaNum[i];
				deltaNum[i] = deltaNum[j];
				deltaNum[j] = holdVal;
			}
		}
	}

	// Print output of delta numbers onto console
	printf("\nYour delta numbers are: ");

	for(i = 0;i < 6;i++) {
		printf("%i ",deltaNum[i]);
	}

	printf("\n\n");
	
	// Random sequence generated to place user numbers in random order 
	i = 0;
	while (randOrder < 6) {
		randPlace = rand() % 6;

		if (finalSeq[randPlace] == 0) {
			finalSeq[randPlace] = deltaNum[i];
			i++;
			randOrder++;
		}
	}
	
	// Output the final, random delta sequence
	printf("Your final delta sequence is: ");

	for(i = 0;i < 6;i++) {
		printf("%i ",finalSeq[i]);
	}

	printf("\n\n");
	
	// Determine the User's lotto number
	lottoNum[0] = finalSeq[0];

	for (i = 1;i < 6;i++) {
		lottoNum[i] = finalSeq[i] + lottoNum[i - 1];
	}

	// Output the final, random delta sequence
	printf("Your lottery numbers are: ");

	for(i = 0;i < 6;i++) {
		printf("%i ",lottoNum[i]);
	}
	

	//Generate winning combinations 1000000 times, or until the user wins
	for(i = 0; i < 1000000;i++) { 
		//generate a delta array similar to the user generated array < 50 total
		totalDelta = 51;
		while (totalDelta > 50) {
			randWin[0] = rand() % 5  + 1; //1 to 5
			randWin[1] = rand() % 10 + 1; //1 to 10
			randWin[2] = rand() % 10 + 1; //1 to 10
			randWin[3] = rand() % 3  + 10;//10 to 12
			randWin[4] = rand() % 8  + 8; //8 to 15
			randWin[5] = rand() % 8  + 8; //8 to 15	
			
			//check if total vlaue of delta numbers exceeds 50
			totalDelta = 0;
			for(j = 0;j < 6;j++) {
				totalDelta = totalDelta + randWin[j];
			}
		}
		//order the winning delta lotto combination ascendingly
		for(j = 0;j < 6;j++) {
			for(k = j + 1;k < 6;k++) {
				if(randWin[j] > randWin[k]) {
					holdVal    = randWin[j];
					randWin[j] = randWin[k];
					randWin[k] = holdVal;
				}
			}
		}

		// delta lotto numbers are added together to generate the winning lotto number 
		for(j = 1;j < 6;j++) {
			randWin[j] = randWin[j] + randWin[j-1];
		}
		
		matchWin = 0;
		//compare values of winning numbers to user lotto numbers
		for(j = 0; j < 6;j++) { 
			if(randWin[j] == lottoNum[j]) {
				matchWin++;
			}
		}
		//break out of loop if winning combination is obtained
		if(matchWin == 6) {
			i = 1000000;
		}
	}
	
	//print win or loss statement
	if(matchWin == 6) {
		printf("\n\nYou got a winning lottery number!");
	} 
	else {
		printf("\n\nYou did NOT get a winning lottery number after 1000000 tries!");
	}

	printf("\n\n");


	return 0;
}
