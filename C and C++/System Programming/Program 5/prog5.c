/*Brandon Shumin
  Program 5
  April 3rd, 2019
  Description:

  Assumptions:
  1) This program assumes that the file being read in is not longer than
     100000 lines.
  2) This program assumes that any given word in the file is not longer than
     100 characters.

  Known Bugs:
  1) Special characters in the read-in file that are not standard ASCII
     characters can, and often does, cause the file to read-in incorrectly
     to the double pointer. Fixing this would require, to my knowledge, a
     function in place to check for these characters and remove them from the
     file during read-in or before read-in. The easier solution is to simply
     not read-in non-standard ASCII characters.
  2) Program randomly started seg faulting after trying to read in the
     USConstitution.txt file, and had very strange problems after that,
     including giving different errors depending on if the file was was being
     run or debugged. I could not fix these problems and the program will
     randomly seg fault sometimes, and sometimes not. I gave up on this
     program after failing to fix the seg fault, though I believe the code
     in everything execpt the search and replace to be correct at least mostly.
     I believe the text files provided to us having erroneous characters played
     a large part into the failure of this program, as I spent the first ~ 8hrs
     on this program just trying to get the file read in to work, only to
     discover that the problem was the text file (There's Something in These
     Hills), as it contained non-standard ASCII characters.


*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

// Function to read in an ASCII text file
char** readASCII(char **text, char *fileName, int *sizePtr);

// Function to allow the user to choice what to do with the text and to exit
void menu(char **text, char * fileIn, char *fileOut, char *fileDict, int *size);

// Function to take in the text and a dictionary file for spellchecking
char** spellCheck(char **text, char *dictName, int *sizeText);

// Function to search and replace words within the textTest
char** searchReplace(char **text);

// Function to save the text to a designated output file
void saveFile(char **text, char *outFile, int sizeText);

int main(int argc, char *argv[]) {
  char** charText = NULL;
  int sizeText;

  // read in the ASCII text file and print when process is successful
  charText = readASCII(charText, argv[1], &sizeText);
  printf("\nFile text successfully loaded!\n");
  // call menu function to loop until an error occurs or the user exits
  menu(charText, argv[1], argv[2], argv[3], &sizeText);

  // if the menu function has been returned without error, program has worked
  printf("\nProgram successfully exited!\n\n");

  return 0;
}

char** readASCII(char **text, char *fileName, int *sizePtr) {
  FILE *inFile;
  char storeChar = ' ', lastChar;
  int lenLine[100000], line = 0, count = 0, i = 0;

  //open file and make sure it is not NULL/unopenable
  inFile = fopen(fileName,"r");
  if (inFile == NULL) {
    fprintf(stderr, "ERROR: Cannot read file, exitting.\n");
    exit(-1); //Terminate program
  }

  // Find the size of each line of text and how many lines of text total
  while(!feof(inFile)) {
    fscanf(inFile,"%c",&storeChar);
    // check for blank lines
    if(storeChar == '\n' && lastChar == '\n') {
      lenLine[line] = 2;
      count = 0;
      line++;
    }
    else if(storeChar == '\n'|| storeChar == EOF){
      lenLine[line] = count;
      count = 0;
      line++;
    }
    else {
      count++;
    }
    // store previous character to allow for checking of blank lines (\n\n)
    lastChar = storeChar;
  }

  //save length of lines for later
  *sizePtr = line;

  //rewind file to re-read
  rewind(inFile);

  //allocate memory for the used double pointer
  text = (char**)malloc((line) * sizeof(char*));
  for(count = 0; count < line; count++) {
    text[count] = (char*)calloc((lenLine[count]), sizeof(char));
  }
  // reset count
  count = 0;
  // Read in data line by line
  while(!feof(inFile)) {
    // storeChar = fgetc(inFile);
    fscanf(inFile,"%c", &storeChar);
    if (storeChar == '\n') {
      text[i][count] = storeChar;
      //printf("%c", text[i][count]);
      i++;
      count = 0;
    } else {
      text[i][count] = storeChar;
      //printf("%c", text[i][count]);
      count++;
    }
    printf("%c", storeChar); // FIXME remove

  }

  //close file
  fclose(inFile);

  //return text read in
  return text;

}

void menu(char** text, char * fileIn, char *fileOut, char *fileDict, int *size) {
  int choice = 0;

  while(choice != 4) {
    printf("\n-----------------------------\n");
    printf("[1] Check spelling of the text\n");
    printf("[2] Search and replace text\n");
    printf("[3] Save the text to an output file\n");
    printf("[4] Exit the program\n");
    printf("Enter a command to execute (1-4): ");
    scanf(" %d", &choice);

    if(choice == 1) {
      // call the spellcheck function
      text = spellCheck(text, fileDict, size);

    } else if(choice == 2)  {
      // call search & replace function
      text = searchReplace(text);
    } else if(choice == 3) {

      // Call the file save function and tell the user when saved successfully
      saveFile(text, fileOut, *size);
      printf("File Saved!\n");

    } else if(choice != 4) {
      printf("Invalid menu selection, try again.\n");
    }
    // clear input buffer
    while(fgetc(stdin) != '\n'){};
  }

  //FIXME
}

char** spellCheck(char **text, char *dictName, int *sizeText) {
  char **dictPtr = NULL, *charPtr, choice, *newDict;
  int sizeDict, i = 0, j = 0, comp = 1, match;

  // read in dictionary
  dictPtr = readASCII(dictPtr, dictName, &sizeDict);

  // printf("%s", text[0]);
  // printf("%s", text[1]);FIXME remove
  // printf("%s", text[2]);
  while(i < *sizeText) {
    printf("HERE:%s\n", text[2]);
    charPtr = strtok(text[i]," ,._-\n\"0123456789\0");
    while(charPtr != NULL) {

      j = 0;
      while(j < sizeDict && comp != 0) {
        comp = strncasecmp(charPtr, dictPtr[j],strlen(charPtr));
        comp == 0 ? (match = 1) : (match = 0);
        j++;
      }
      choice = ' ';
      if (match == 0) {
        printf("The word \"%s\" not in dictionary.\n", charPtr);
        printf("Would you like to ignore the word? (y/n)\n");
        // clear input buffer and get choice for dictionary
        while(fgetc(stdin) != '\n'){};
        while(choice!='y' && choice!='Y' && choice!='n' && choice!='N') {
          fscanf(stdin,"%c",&choice);
          if ((choice == 'y' || choice == 'Y') && (newDict != NULL)) {
            printf("Adding the word to a personal Dictionary.\n");
            printf("Enter name of the personal dictionary file: ");
            scanf("%s", newDict);
            // FIXME add new word to dictionary
          } else if((choice == 'n' || choice == 'N')) {
             
          }

        }
          // create a new dictionary file by copying the old Dictionary
          // file and adding the new word plus a new line character
      }

    }
    i++;
  }
      charPtr = strtok(NULL, " ,._-\n\"0123456789\0");
}




  //   return text;
  // }
  char** searchReplace(char **text) {
    char *searchFor = malloc(100*sizeof(char));

    printf("Enter word to search for: ");
    fscanf(stdin,"%s", searchFor);
    return text;
  }
//   int match, count = 0, lenText;
//   int lineCount = 0, i, character = 0;
//   char storeChar = ' ', *storeWord = malloc(sizeof(char));
//   //Read in dictionary file
//
//   while(storeChar != EOF) {
//     storeChar = dictPtr[0][count];
//     if (storeChar == '\n') {
//       lineCount++;
//       count++;
//     } else {
//
//     count++;
//
//     }
//   }
//
//   count = 0;
//   while(count < lenText) {
//     storeChar = text[0][count];
//     storeWord[character] = storeChar;
//     if(storeChar == ' ' || storeChar == '\n' || storeChar == '.' ||
//        storeChar == '!' || storeChar == '?' || storeChar == ',') {
//          printf("HERE\n");
//          match = 0;
//          // storeWord[character] = storeChar;
//          if(strlen(storeWord) > 1) {
//            for(i = 0; i < lineCount; i++) {
//              if(strcasecmp(storeWord,dictPtr[i]) == 1) {
//                match = 1;
//                printf("MATCH\n");
//              }
//            }
//          }
//          //handle match here
//          count++;
//          character = 0;
//          storeWord = malloc(count + sizeof(char));
//     } else {
//       storeWord = realloc(storeWord, count + sizeof(char));
//       count++;
//       character++;
//     }
//   }
// }

void saveFile(char **text, char *outFile, int sizeText) {
  FILE *fileOut;
  char *storeLine = NULL;
  int line = 0;
  // open file for read in and check if file is unopenable. Error if so
  fileOut = fopen(outFile, "w");
  if (outFile == NULL) {
    fprintf(stderr, "ERROR: Cannot read file, exitting.\n");
    exit(-1); //Terminate program
  }

  // iterate through every character in the file by turning the 2-D array into
  // a 1-D array by leaving the first parameter at 0 and wrapping the index
  while(line < sizeText) {
    strcpy(storeLine,text[line]);
    fprintf(fileOut,"%s",text[line]);
  }

  // close file
  fclose(fileOut);
}
