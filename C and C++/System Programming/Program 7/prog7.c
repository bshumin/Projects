/*
    Brandon Shumin
    Project 7
    ECE 2220
    April 18th, 2019
    Program Description:

    Assumptions:

    Bugs: None known.
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>

#define TERM 4

// global variable for alarm time shared between all child processes
int alrmTime = 0;

// global variable used to refuel the subs upon request
int addFuel = 0;

// global variable for launching missiles from subs
int launch = 0;

// handle timer for alarm
void alarmTime(int sig);

// handle launching sub missiles upon request of base station
void launchMissile(int sig);

// handle refueling subs upon request of base station
void refuelSub(int sig);

// find if a specified terminal is open, and return a 1 if open, otherwise 0
int tryTerminal(int term);

// carry out all the required processes for a given child processes
void childProcess(int idStore, int subNum);

// carry out required processses for the parent process
void parentProcess(int idStore, int *subPid);

int main(void){
  pid_t pid;
  int i = 0, j = 0, *idStore = (int*)calloc(TERM,sizeof(int));
  int status, *subPid = (int*)calloc(TERM-1,sizeof(int));
  time_t rawtime;
  struct tm *timeInfo;
  char timeBuffer[80];

  // //find which terminals are being used by finding first 4 terminals open FIXME remove comment
  // for(i=1; i<=99; i++) {
  //   status = tryTerminal(i);
  //
  //   //if the terminal is open, store the terminal number to a pointer array
  //   if(status == 1) {
  //     idStore[j] = i;
  //     j++;
  //   }
  // }

  printf("Enter in the four terminals numbers with the first being the main terminal.\n");
  scanf(" %d",&idStore[0]);
  scanf(" %d",&idStore[1]);
  scanf(" %d",&idStore[2]);
  scanf(" %d",&idStore[3]);

  //check if less than 4 terminals found, of so, error
  if(idStore[0]==0||idStore[1]==0||idStore[2]==0||idStore[3]==0) {
    fprintf(stderr,"ERROR: 4 terminals must be open for this program to run\n");
    exit(-1);
  }

  //time printout before child processes made
  time(&rawtime);
  timeInfo = localtime(&rawtime);
  strftime(timeBuffer,80,"%H:%M:%S", timeInfo);
  fprintf(stdout,"Time : %s\n", timeBuffer);


  //create 3 child processes from 1 parent, and separate the child and parent
  for(i = 1; i < TERM; i++){
     pid = fork();

     //child process
     if(pid == 0){
       srand(time(NULL)+i);
       childProcess(idStore[i],i);
     } else{
       subPid[i-1] = pid;
     }
   }

   //parent process
   parentProcess(idStore[0], subPid);

  return 0;
}

void alarmTime(int sig){

  alrmTime++;

}

void launchMissile(int sig){

  launch = 1;

}

void refuelSub(int sig) {

  addFuel = 1;

}

int tryTerminal(int term) {
  FILE *terminal;
  char *termName = (char*)calloc(13,sizeof(char));
  int status = 0;

  // check if specified terminal is open
  sprintf(termName,"/dev/pts/%d", term);
  terminal = fopen(termName, "w");

  if(terminal != NULL){
    fprintf(terminal, "Using this terminal!\n");
    status = 1;

    // close opened terminal
    fclose(terminal);
  }

  // return if the terminal is open or not
  return status;

}

void childProcess(int idStore, int subNum){
  int fuel = (rand() % 4001) + 1000, missiles = (rand() % 5) + 6, dist = 0;
  int  direction = 0; // direction to or from base, 0 = from, 1 = to base
  char *termName = (char*)calloc(13,sizeof(char));
  FILE *terminal;
  time_t rawtime;
  struct tm *timeInfo;
  char timeBuffer[80];


  //install alarm handlers for child process
  signal(SIGALRM, alarmTime);
  signal(SIGUSR1, launchMissile);
  signal(SIGUSR2, refuelSub);
  alarm(1);

  //open relevant terminal to print data
  sprintf(termName,"/dev/pts/%d", idStore);
  terminal = fopen(termName, "w");

  // keep child process alive until child dies
  while(1){

    // terminate child if sub is sucessfully back at base
    if(dist <= 0 && direction == 1){
      fprintf(terminal,"\nSub %d safely returned to base!\n",idStore);
      exit(0);
    }

    pause();
    alarm(0);

    //check if sub has been requested to be refueled by base station
    if(addFuel == 1){
      fuel = (rand() % 4001) + 1000;
      addFuel = 0;
    }
    //check if sub has been requested to launch missile by base station
    if(launch == 1){
      if(missiles > 0) {
        missiles--;
        fprintf(terminal,"\nSub %d launched missile! %d missiles left\n", subNum, missiles);
      }
      if(missiles == 0){
        direction = 1;
        fprintf(terminal,"\nSub %d out of ordinance, returning to base.\n", subNum);
      }
      launch = 0;
    }

    // reduce fuel by 100-200 gallons every second
    fuel -= (rand() % 101) + 100;

    //change distance every 2 seconds
    if((alrmTime % 2) == 0) {
      //if traveling to base, subtract 3 to 8 miles
      if(direction == 1) {
        dist -= (rand() % 6) + 3;
      // if traveling towards base, add 5 to 10 miles
      } else {
        dist += (rand() % 6) + 5;
      }
    }

    //print out time and sub status to terminal
    if((alrmTime % 3) == 0) {

      //print out time to terminal
      time(&rawtime);
      timeInfo = localtime(&rawtime);
      //printf formatted status and resources
      strftime(timeBuffer,80,"%H:%M:%S", timeInfo);
      fprintf(terminal,"\nTime : %s\n", timeBuffer);
      fprintf(terminal,"\tSub %d to base, %d gallons left,",subNum, fuel);
      fprintf(terminal," %d missiles left", missiles);
      fprintf(terminal,"\n\t%d miles from base.\n", dist);


    }

    if(fuel <= 0) {
      //print sub is dead in water and send signal to parent and kill process
      fprintf(terminal,"\nSub %d dead in the water.\n", subNum);
      kill(getppid(), SIGUSR1);
      exit(-1);
    } else if(fuel < 500){
      //warn that sub is close to out of fuel
      fprintf(terminal,"\nSub %d running out of fuel!\n", subNum);
    }

    alarm(1);
  }
  //clse terminal after data is written
  fclose(terminal);
}


void parentProcess(int idStore, int *subPid) {
  char choice = ' ';
  int val = 0;

  //install signal handler for child-sent signals
  signal(SIGALRM, alarmTime); //FIXME change signal function handler
  //signal(SIGCHLD, );

  do {

    //loop until valid command is entered
    while((choice != 'l' && choice !='r' && choice != 's') || (val != 1 && val != 2 && val != 3)) {
      fprintf(stdout,"Enter a command(ln,rn,sn,q): ");
      fscanf(stdin,"%c", &choice);

      //kill all processes if q is entered
      if(choice == 'q'){
        fprintf(stdout, "\nTerminating remaining child processes.\n");
        kill(subPid[0], SIGTERM);
        kill(subPid[1], SIGTERM);
        kill(subPid[2], SIGTERM);
        fprintf(stdout, "Terminating parent process.\n");
        exit(0);
      }
        fscanf(stdin,"%d",&val);
        if((choice != 'l' && choice !='r' && choice != 's') || (val != 1 && val != 2 && val != 3)) {
          fprintf(stdout, "Invalid command, please try again.\n\n");
        }
    }
    // send signal based on user choice
    if(choice == 'l') {
      kill(subPid[val-1], SIGUSR1);
    } else if(choice == 'r') {
      kill(subPid[val-1], SIGUSR2);
    } else if(choice == 's') {
      kill(subPid[val-1], SIGTERM);
    }

  } while(1);
}
