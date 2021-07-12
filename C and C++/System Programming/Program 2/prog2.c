// Brandon Shumin                                                               //
// Project 2                                                                    //
// ECE 2220                                                                     //
// Febuary 3, 2019                                                              //
//------------------------------------------------------------------------------//
// Program Description: This program allows the user to input various           //
//                      information about a politician, such as name, age, home //
//                      state, etc., by using a menu and then allows the user   //
//                      to display the data in a formatted way, and also allows //
//                      the user to clear all stored data, allowing more data   //
//                      to be entered still.                                    //
//------------------------------------------------------------------------------//

#include<string.h>
#include<stdio.h>
#include<stdlib.h>

// function provides user with menu options and returns a value for the user choice
int menu(void);

// function gets the politician's first, middle and last name from the user
char enterName(char *firstName, char *lastName, char *suffix, int size);

// function gets the number of years that the politician has served
int enterYear(void);

// function gets position, office, and party of the politician
int enterPOS(char *office, char *state);

// function gets the age and sex of politician
int enterAgeSex(char *sex);

// function gets the contacts of the politician
void enterContacts(char *twitterAcc, char *phoneNum);

// function gets the amount of money taken by the politician
float enterContributions(void);

// function gets the amount of lies told by the politcian
float enterLies(void);

// function displays data about the politician
void displayData(char *firstName, char middleInit, char *lastName, char *suffix,
                 int year, int party, char *office, char *state, int age, char *sex,
					       char *twitterAcc, char *phoneNum, float dollarsTaken, float liesTold);

// function clears all character pointers
void clearData(char *firstName, char *lastName, char *suffix, char *office, char *state,
               char *sex, char *twitterAcc, char *phoneNum);

int main(void) {
	int menuChoice = 1, year = -1, party = -1, age = -1;
  char *firstName = malloc(32), *lastName = malloc(32);
	char *suffix = malloc(3), *office = malloc(32), *state = malloc(3), *sex = malloc(1);
	char *twitterAcc = malloc(16), *phoneNum = malloc(13);
	char middleInit = ' ';
	float dollarsTaken = -1, liesTold = -1;

	// keep redisplaying the menu, so long as the user does select quit (8)
	while(menuChoice != 8) {

		// Get user menu choice
		menuChoice = menu();

		// call function that matches user's desired function/informatiion
		if(menuChoice == 1) {

			// call function to get politician's first, middle and last name
			middleInit = enterName(firstName, lastName, suffix, 32);

		} else if(menuChoice == 2) {

			// call function to get the years that the politician has been in politics
			year = enterYear();
			// call function to get politician's office, home state, and political party
			party = enterPOS(office, state);

		} else if(menuChoice == 3) {

			// call function to get politician's age and sex
			age = enterAgeSex(sex);

		} else if(menuChoice == 4) {

			// call function to get politician's contacts
			enterContacts(twitterAcc, phoneNum);

		} else if(menuChoice == 5) {

			// call funtion to get politician's money taken
			dollarsTaken = enterContributions();

			// call function to get number of lies told by politician
			liesTold = enterLies();

		} else if(menuChoice == 6) {

			// call function to display all data
			displayData(firstName, middleInit, lastName, suffix, year, party, office, state,
		 	            age, sex, twitterAcc, phoneNum, dollarsTaken, liesTold);

		} else if(menuChoice == 7) {

			//set all integer and float values to -1 ( clear value for display purposes)
			year = -1;
			party = -1;
			age = -1;
			dollarsTaken = -1;
			liesTold = -1;

			//clear character values
			middleInit = ' ';

			clearData(firstName, lastName, suffix, office, state, sex, twitterAcc, phoneNum);

		}

	}

	return 0;

}

int menu(void) {
	int menuChoice = 1;


	//Display menu Options until a valid selection has been made
	do {
		//check if the user input an invalid menu choice, and if so, let them know
		if(menuChoice < 1 || menuChoice > 8) {
			fprintf(stderr,"\nERROR: Invalid menu selection, select a number from 1 to 8.\n");
		}

		//Display the user's menu choices
		printf("\nSelect a data type to input for the politician (1-8):\n");
		printf("1-Enter Name\n2-Enter Years, Party, Office, and State\n");
		printf("3-Enter Age and Sex\n4-Enter Contacts\n5-Enter Contributions and Lies\n");
		printf("6-Display the Data\n7-Clear all Data\n8-Quit\n\n");

		//Read in the user's menu choice
		printf("Menu Selection (1-8): ");
		scanf(" %d",&menuChoice);

		//clear input buffer
		while((getchar()) != '\n');

	} while(menuChoice < 1 || menuChoice > 8);

	//return the value of the user's menu choice
	return menuChoice;

}

char enterName(char *firstName, char *lastName, char *suffix, int size) {
	char middleInit = 'a';

	// User is prompted to enter politician's first name
	do {

		printf("\nEnter politician's first name (32 character maximum): ");
		scanf(" %s", firstName);

		//if the name input is longer than 32 character maximum, loop and send error message
		if (strlen(firstName) > 32) {

			fprintf(stderr, "\nERROR: First name exceeds 32 character maximum.");

		}

	} while (strlen(firstName) > 32);

	//clear input buffer
	while((getchar()) != '\n');

	// User is prompted to enter politician's middle initial, or tell if there is none
	printf("\nEnter politician's middle initial (Type ? if no middle initial): ");
	scanf(" %c",&middleInit);

	// if the user denotes that the politician has no middle initial, store a null value
	if(middleInit == '?') {

		middleInit = '\0';

	}
	//clear input buffer
	while((getchar()) != '\n');

	// User is prompted to enter politician's last name
	do {

		printf("\nEnter politician's last name (32 character maximum): ");
		scanf(" %s", lastName);

		//if the name input is longer than 32 character maximum, loop and send error message
		if (strlen(lastName) > 32) {

			fprintf(stderr, "\nERROR: Last name exceeds 32 character maximum.");

		}

	} while (strlen(lastName) > 32);

	// user is prompted to enter politician's suffix if they have one
	do {

		printf("\nEnter the politician's suffix (Type ? if no suffix): ");
		scanf(" %s", suffix);

		//if the suffix input is longer than 3 characters, loop and send error message
		if (strlen(suffix) > 3) {

			fprintf(stderr, "\nERROR: Suffix exceeds 3 character maximum.");

		}

	} while (strlen(suffix) > 3);

	//clear input buffer
	while((getchar()) != '\n');

	// if user denotes no suffix, make suffix variable null
	if(strstr(suffix,"?")) {

		suffix[0] = '\0';

	}

	// return the politician's middle initial
	return middleInit;
}

int enterYear(void) {
	int year;

	//user is prompted to enter the years the politician has served
	do {

			printf("\nEnter the number of years served (0-99): ");
			scanf(" %d",&year);

			//if the year entered is invalid, loop and send error message
			if (year > 99 || year < 0) {

				fprintf(stderr, "\nERROR: Enter a year between 0 and 99.");

			}

		} while(year > 99 || year < 0);

	return year;
}

int enterPOS(char *office, char *state) {
	int partyNum, i, j, stateNum;
	char *stateList[100] = {"Alabama","AL","Alaska","AK","Arizona","AZ","Arkansas","AR",
								  "California","CA","Colorado","CO","Connecticut","CT","Delaware",
								  "DE","Florida","FL","Georgia","GA","Hawaii","HI","Idaho","ID",
								  "Illinois","IL","Indiana","IN","Iowa","IA","Kansas","KS",
								  "Kentucky","KY","Louisiana","LA","Maine","ME","Maryland","MD",
								  "Massachusetts","MA","Michigan","MI","Minnesota","MN",
								  "Mississippi","MS","Missouri","MO","Montana","MT","Nebraska","NE",
								  "Nevada","NV","New Hampshire","NH","New Jersey","NJ","New Mexico",
								  "NM","New York","NY","North Carolina","NC","North Dakota","ND",
								  "Ohio","OH","Oklahoma","OK","Oregon","OR","Pennsylvania","PA",
								  "Rhode Island","RI","South Carolina","SC","South Dakota","SD",
								  "Tennessee","TN","Texas","TX","Utah","UT","Vermont","VT",
								  "Virginia","VA","Washington","WA","West Virginia","WV",
								  "Wisconsin","WI","Wyoming","WY"};

	//user is prompted to enter the politician's held office until the input is valid length
	do {

		printf("\nEnter the office held by the politician: ");
		scanf(" %s", office);

		//clear input buffer
		while((getchar()) != '\n');

		//if the office name exceeds 32 characters, loop and print error message
		if (strlen(office) > 32) {

			fprintf(stderr, "\nERROR: Name of office exceeds 32 character maximum.");

		}

	} while (strlen(office) > 32);


	//user is prompted for party of politician and until value is made sure to be in range
	do {

		//Have user enter politician's party by selecting a number
		printf("\nEnter the number that corresponds to the politician's political party.\n");
		printf("1-Constitution,2-Democrat,3-Green,4-Libertarian,5-Republican: ");
		scanf(" %d", &partyNum);

		//clear input buffer
		while((getchar()) != '\n');

		//send error message if user selects an invalid value
		if (partyNum > 5 || partyNum < 1) {
			fprintf(stderr,"\nERROR: Select a value within the range (1-5).");
		}

	} while (partyNum > 5 || partyNum < 1);

	//user is prompted to pick a home state from the list
	printf("\nEnter the politician's home state by choosing a number from the list: \n\n");

	// display the number representing the alphebetical place of the state, then the state
	for(i = 0;i < 10;i++) {

		for(j = 1;j <= 5;j++) {

			// print out the state names with corresponding numbers
			printf("%d-%s", (i * 5) + j, stateList[2 * j +10 * i - 2]);

			// print comma after every state selection except the last one
			if (j != 5 || i != 9) {
				printf(", ");
			}

		}

		printf("\n");

	}

	// print newline for formatting
	printf("\n");

	// check if user made a choice outside the range of the state list, but always run once
	do {

		//ask for user selection and save selection to stateList
		printf("Selection (1-50): ");
		scanf(" %i", &stateNum);

		// send error message if user makes invalid choice
		if (stateNum < 1 || stateNum > 50) {

			fprintf(stderr,"\nERROR: Select a number for the corresponding state from 1-50.\n");

		}

	} while(stateNum < 1 || stateNum > 50);

	//save user selected state abbreviation to state sstring bassed off of selection value
	strcpy(state,stateList[(stateNum * 2) - 1]);

	//return the value of the politician's party number
	return partyNum;
}

int enterAgeSex(char *sex) {
	int age = 0;

	// loop user sex selection as long as the input is invalid, but triggering at least once
	do {

		// get user input of politician's sex
		printf("\nEnter politician's sex (M/F): ");
		scanf(" %s", sex);

		//clear input buffer
		while((getchar()) != '\n');

		// send error if user inputs an invalid sex
		if ((strcmp(sex, "M") != 0) && strcmp(sex, "F") != 0) {
			if ((strcmp(sex, "m") != 0) && strcmp(sex, "f") != 0) {

				fprintf(stderr,"\nERROR: Pick a valid sex of either M or F.");

			}

		}

	} while (((strcmp(sex,"M") != 0) && strcmp(sex, "F") != 0) && ((strcmp(sex,"m") != 0) &&
			   strcmp(sex, "f") != 0));

	//print newline for formatting
	printf("\n");

	// loop user input of age as long as the input is invalid, but triggering at least once
	do {

		// get user input of politician's age
		printf("Enter politician's age (21-99): ");
		scanf(" %i", &age);

		// print error message if user input is outside of desired range
		if (age > 99 || age < 21) {

			fprintf(stderr,"\nERROR: Please enter an age between 21 and 99.\n");

		}

	} while (age > 99 || age < 21);

	return age;
}

void enterContacts(char *twitterAcc, char *phoneNum) {
	int numMatch = 1;

	// prompt the user for the politician's twitter account name
	do {

		printf("\nEnter the politician's twitter account name (do not include @ or spaces): ");
		scanf(" %s", twitterAcc);

		//clear input buffer
		while((getchar()) != '\n');

		// if twitter account name exceeds character maximum, loop and display error message
		if (strlen(twitterAcc) > 15) {

			fprintf(stderr, "\nERROR: Twitter name exceeds 15 character maximum.");

		}

	} while(strlen(twitterAcc) > 15);

	//prompt user for politician's phone number until valid number and format are found
	do {

		printf("\nEnter politician's ten-digit phone number e.g.(111)222-3333: ");
		scanf(" %s", phoneNum);

		// if phone number does not have proper formatting or length, loop and display error
		if ((strstr(phoneNum,"(") && strstr(phoneNum,")") && strstr(phoneNum,"-")) &&
			(strlen(phoneNum) <= 13)) {

				numMatch = 0;

		} else if (strlen(phoneNum) > 13) {

			fprintf(stderr, "\nERROR: Phone number entered exceeds 13 characters.");

		} else {

			fprintf(stderr,"\nERROR: Invalid format. Enter phone number in the given format.");

		}

		//clear input buffer
		while((getchar()) != '\n');

	} while (numMatch);

}

float enterContributions(void) {
	float money;

	// prompt user at least once, but until valid politician money taken value is entered
	do {

		printf("\nEnter the amount of money taken by the politician (in USD): ");
		scanf("%f", &money);

		// output error message if value is outside range
		if ((money > 1.0e50) || (money < 0)) {

			fprintf(stderr, "\nERROR: Value must be between 0 and 1.0e50.");

		}

	} while ((money > 1.0e50) || (money < 0));


	return money;
}

float enterLies(void) {
	float lies;

// prompt user at least once, but until valid politician lies value is entered
	do {

		printf("\nEnter the amount of lies told by the politician: ");
		scanf("%f", &lies);

		// output error message if value is outside range
		if ((lies > 1.0e100) || (lies < 0)) {

			fprintf(stderr, "\nERROR: Value must be between 0 and 1.0e100.");

		}

	} while ((lies > 1.0e100) || (lies < 0));



	return lies;

}

void displayData(char *firstName, char middleInit, char *lastName, char *suffix,
                 int year, int party, char *office, char *state, int age, char *sex,
					  char *twitterAcc, char *phoneNum, float dollarsTaken, float liesTold) {

	// display data in a formatted way
	printf("\n----------------------------------------------------------------\n");
	printf("DISPLAYING DATA FOR POLITICIAN:\n");
	printf("Full Name: %s %c %s %s\n", firstName, middleInit, lastName, suffix);
	printf("Years Served: ");

	// if year had no data entered, denoted by a -1, leave info blank
	if (year != -1) {

		printf("%i\n", year);

	} else {

		printf("\n");

	}

	printf("Political Party: ");

	// turn numeric representation of political party back into related string
	if (party == 1) {

		printf("Constitution");

	} else if (party == 2) {

		printf("Democrat");

	} else if(party == 3) {

		printf("Green");

	} else if(party == 4) {

		printf("Libertarian");

	} else if(party == 5) {

		printf("Republican");

	}

	// print newline for formatting purposes
	printf("\n");

	// continue displaying data
	printf("Office Held: %s\n", office);
	printf("Home State: %s\n", state);
	printf("Age: ");

	// if age has been modified by the user, display age
	if (age != -1) {

		printf("%i\n", age);

	} else {

		printf("\n");

	}

	//continue displaying data
	printf("Sex: %s\n", sex);
	printf("Twitter Account: @%s\n", twitterAcc);
	printf("Phone Number: %s\n", phoneNum);
	printf("US Dollars Received: ");

	// if dollarsTaken was entered by user, display the data
	if (dollarsTaken != -1) {

		printf("%0g\n", dollarsTaken);

	} else {

		printf("\n");

	}


	printf("Number of Lies Told: ");

	// if liestold has been entered by user, display the data
	if (liesTold != -1) {

	printf("%0g\n",liesTold);

	} else {

		printf("\n");

	}

	//if the number is not undefined, display ratio of contributions to lies told
	printf("Contributions to Lies Told ($/Lie): ");

	if (liesTold > 0) {

		printf("%0.2g\n",	dollarsTaken/liesTold);

	}

	// formatting line
	printf("\n----------------------------------------------------------------\n");

}

void clearData(char *firstName, char *lastName, char *suffix, char *office, char *state,
               char *sex, char *twitterAcc, char *phoneNum) {

	// give all character pointers blank values
	strcpy(firstName, " ");
	strcpy(lastName, " ");
	strcpy(suffix, " ");
	strcpy(office, " ");
	strcpy(state, " ");
	strcpy(sex, " ");
	strcpy(twitterAcc, " ");
	strcpy(phoneNum, " ");

}
