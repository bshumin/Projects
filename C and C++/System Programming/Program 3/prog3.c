// Brandon Shumin                                                                  //
// Program 3                                                                       //
// ECE 2220                                                                        //
// February 19, 2019                                                               //
//---------------------------------------------------------------------------------//
// Program Description: This program asks for the user to enter two characters     //
//                      and then calculates 5 parity bits based off of certain     //
//                      bits within the binary values for the entered characters.  //
//                      these parity bits are then inserted into the value for the //
//                      initial characters and printed out along with the orginal  //
//                      codeword.                                                  // 
//---------------------------------------------------------------------------------//


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// function to make all characters in a string uppercase
void upCase(char *strIn);

// function to add parity bits to the codeword
long insertParity(long codeWord);

// function to print out the formatted codeword bits
void printCode(long codeWord, long parityCodeWord, char *userIn);

int main(void) {
	
	long codeWord, parityCodeWord;
	char *keyPhrase = "EXIT", *userIn = malloc(32), *upCaseIn = malloc(32);	

	// print project specifications as asked fro in the programming assignment
	printf("\nThis program either takes in two characters and creates a parity checking\n");
   printf("code word from those characters, or exits if the keyword is entered.");

	// ask for new characters as long as keyword is not entered
	do { 
		// set user 32-bit code to 0 on every run of the loop
		codeWord = 0;

		// get user input
		printf("\nEnter 2 characters or enter 'exit' to end program: ");
		scanf(" %s", userIn);
	
		// store keyword in dummy variable and change to upcase to check for 'exit'
		strcpy(upCaseIn, userIn);
		upCase(upCaseIn);

		// if user enters an invalid input, display error and loop
		if((strlen(userIn) != 2) && (strcmp(upCaseIn,keyPhrase) != 0)) {
			
			fprintf(stderr, "ERROR: Invalid input, enter 2 characters or 'exit'.\n");
		
		// if keyphrase is entered skip the else statement
		} else if (strcmp(upCaseIn,keyPhrase) == 0) {

		} else {
			
			// store user input chars as a single 32-bit integer for required bitwise operations
			codeWord = userIn[1];
			codeWord = (codeWord << 8) + userIn[0];
			
			// call function to perform relevant operations
			parityCodeWord = insertParity(codeWord);
			
			// call function to display original codeword and codeword with parity bits
			printCode(codeWord, parityCodeWord, userIn);
			
		}

	} while(strcmp(keyPhrase, upCaseIn) != 0); 
	


	return 0;
}

void upCase(char *strIn) {
	int i, length;
	
	length = strlen(strIn);
	
	// for each character in a string, check if it is lowercase, if so, uppercase it
	for(i = 0;i < length;i++) {
		
		if (strIn[i] >= 97 && strIn[i] <= 122) {
			
			// if lowercase, subtract 32 from the ASCII value to uppercase it
			strIn[i] -= 32;

		}	
	}
}

long insertParity(long codeWord) {
	int parBit1, parBit2, parBit4, parBit8, parBit16, parSum = 0;
	long comp = 1, parityCodeWord = 0;
	//NOTE: simplified if statements used in this function for readability and compactness
	// initialize all parity bits and parity sum to 0
	parBit1 = parBit2 = parBit4 = parBit8 = parBit16 = parSum = 0;

	// determine the parity value for parity bit 1
	(comp & codeWord) ? parSum += 1:parSum;//bit 1 in codeword
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 2
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 4
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 5
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 7
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 9
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 11
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 12
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 14
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 16

	
	// 1st parity bit is equal to the modulus of parSum, reinitialize parSum to 0
	parBit1 = parSum % 2;
	parSum = 0;
	comp = 1;

	// determine parity value for parity bit 2
	(comp & codeWord) ? parSum += 1:parSum;//bit 1 in codeword
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 3
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 4
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 6
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 7
	comp = comp << 3;
	(comp & codeWord) ? parSum += 1:parSum;//bit 10
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 11
	comp = comp << 2;
	(comp & codeWord) ? parSum += 1:parSum;//bit 13
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 14
	
	// 2nd parity bit is equal to the modulus of parSum, reinitialize parSum to 0
	parBit2 = parSum % 2;
	parBit2 = parBit2 << 1;
	parSum = 0;
	comp = 2;

	// determin parity value for parity bit 4
	(comp & codeWord) ? parSum += 1:parSum;//bit 2 in codeword
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 3
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 4
	comp = comp << 4;
	(comp & codeWord) ? parSum += 1:parSum;//bit 8
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 9
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 10
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 11
	comp = comp << 4;
	(comp & codeWord) ? parSum += 1:parSum;//bit 15
	comp = comp << 1;
	(comp & comp <<1) ? parSum += 1:parSum;//bit 16

	// 4th parity bit is equal to modulus of parSum, reinitialize parSum to 0
	parBit4 = parSum % 2;
	parBit4 = parBit4 << 3;
	parSum = 0;
	comp = 16;

	// determine parity value for parity bit 8
	(comp & codeWord) ? parSum += 1:parSum;//bit 5 in codeword
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 6
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 7
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 8
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 9
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 10
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 11

	// 8th parity bit is equal to modulus of parSum, reinitialize parSum to 0
	parBit8 = parSum % 2;
	parBit8 = parBit8 << 7;
	parSum = 0;
	comp = 2048;
	
	// determine parity value for parity bit 16
	(comp & codeWord) ? parSum += 1:parSum;//bit 12 in codeword
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 13
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 14
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 15
	comp = comp << 1;
	(comp & codeWord) ? parSum += 1:parSum;//bit 16

	// 16th parity bit is equal to modulus of parSum, reinitialize parSum to 0
	parBit16 = parSum % 2;
	parBit16 = parBit16 << 15;
	parSum = 0;
	//shuft codeword to allow for accurate bitwise comparison
	codeWord = codeWord << 5;

	// insert parity bits into relevant spots within parityCodeWord
	for(comp = 1<<20;comp > 0; comp = comp >> 1) {
		if(comp == 1) {
			parityCodeWord = parityCodeWord | parBit1;
			codeWord = codeWord >> 1;
		} else if(comp == 2) {
			parityCodeWord = parityCodeWord | parBit2;
			codeWord = codeWord >> 1;
		} else if(comp == 8) {
			parityCodeWord = parityCodeWord | parBit4;
			codeWord = codeWord >> 1;
		} else if(comp == 128) {
			parityCodeWord = parityCodeWord | parBit8;
			codeWord = codeWord >> 1;
		} else if(comp == 32768) {
			parityCodeWord = parityCodeWord | parBit16;
			codeWord = codeWord >> 1;
		} else if(codeWord & comp) {
			parityCodeWord = parityCodeWord | comp;
		}
	}
	
	// return the codeword with parity bits inserted into relevant positions
	return parityCodeWord;
}

void printCode(long codeWord, long parityCodeWord, char *userIn) {
	long comp = 1;

	//print out orginal codeword without parity bits
	//Spaces used intead of \t to ensure formatting is the same regardless of set tabstops
	printf("\nInitial Input Code:\n");
	printf("                         %c        %c\n", userIn[1], userIn[0]);
	printf("0x    00       00       %X       %X\n", userIn[1], userIn[0]);
	printf("-------- -------- ");
	
	//print the binary value saved in codeWord
	for(comp = (comp << 15);comp > 0; comp = (comp >> 1)) {
	
		if (comp & codeWord) {
			printf("1");
		} else {
			printf("0");
		}

		// print a space between the first 8 bits and the second 8 bits
		if (comp == 256) {
			printf(" ");
		}

	}
	
	printf("\n\n");
	
	// print out original code with parity bit markers 
	printf("Partially Filled Code: \n\n");
	printf("-------- ---");

	//reinitialize comp
	comp = 1;

	//print binary bits in codeWord separated by a 'P' for parity bits
	for(comp = (comp << 15); comp > 0; comp = (comp >> 1)) {
	
		if (comp & codeWord) {
			printf("1");
		} else {
			printf("0");
		}
		
		// print a space between the first 8 bits and the second 8 bits
		if (comp == 16 || comp == 2048) {
			printf(" ");
		}
		
		if (comp == 2048 || comp == 16  || comp == 2) {
			printf("P");
		} else if (comp == 1) {
			printf("PP");
		}

	}
	
	printf("\n\n");

	// reinitialize comp
	comp = 1;

	// print binary codeWord with parity bits inserted
	printf("Final Code Word with Parity Bits:\n");
	printf("\n-------- ---");
	
	for(comp = (comp << 20); comp > 0; comp = (comp >> 1)) {
	
		if (comp & parityCodeWord) {
			printf("1");
		} else {
			printf("0");
		}
		
		// print a space between the first 8 bits and the second 8 bits
		if (comp == 256 || comp == 65536) {
			printf(" ");
		}
	}
	//print hex format of parity code
	printf("\n                                    = %#010X", (int)parityCodeWord);
}

	
