// USAGE:
// ./a.out -[routine type] < [file_name.txt]
// where [routine type] is either rr, sjf, or fifo


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 26
typedef struct task{
  int
    task_id,       /* alphabetic tid can be obtained as 'A'+(task_counter++) */
    arrival_time,
    service_time,
    remaining_time,
    completion_time,
    response_time,
    wait_time,
    printed,
    on_ready;
  struct task *next;
}Task;

int main(int argc, char **argv){
  int sched = 0, i, j, ready_list, task_counter = 0, current_time = 0, min_serv = 0;
  int thread_choice, remaining_tasks, running = 0, found_next_task = 0, max_serv;
  Task *tasks[MAX_TASKS];
  Task *rover, *temp;

  // allocate space for tasks
  for(i=0;i<MAX_TASKS;i++){
    tasks[i] = (Task *)malloc(sizeof(struct task));
  }

  // select the correct scheduler
  if(strcmp(argv[1],"-fifo") == 0){
    sched = 0;
  } else if(strcmp(argv[1],"-sjf") == 0){
    sched = 1;
  } else if(strcmp(argv[1],"-rr") == 0){
    sched = 2;
  } else{
    printf("\n Not a valid scheduling policy. Select -fifo, -sjf, or -rr.\n");
    exit(0); // terminate program if invalid scheduler selected
  }
  // ./a.out -rr < in2.txt
  // read in information from stdin from file
  while(fscanf(stdin,"%d %d", &tasks[task_counter]->arrival_time, &tasks[task_counter]->service_time) && !feof(stdin) && task_counter < 25) {
    tasks[task_counter]->task_id = 'A' + (task_counter);
    tasks[task_counter]->next = tasks[task_counter+1];
    tasks[task_counter]->remaining_time = tasks[task_counter]->service_time;
    tasks[task_counter]->printed = 0;
    tasks[task_counter]->on_ready = 0;
    task_counter++;
  }

  // set the last next value in the linked list to NULL and set tid
  tasks[task_counter]->next=NULL;
  tasks[task_counter]->task_id = 'A' + (task_counter);
  tasks[task_counter]->remaining_time = tasks[task_counter]->service_time;
  tasks[task_counter]->printed = 0;
  tasks[task_counter]->on_ready = 0;

  // determine what to do based on which selected scheduler
  if(sched == 0){ // FIFO case handler
    printf("FIFO scheduling results\n\n");
    printf("time  cpu  ready queue (tid/rst)\n");
    printf("----  ---  ---------------------\n");
    for(i=0;i<=task_counter;i++){
      // while no thread is running due to not arriving yet, increment time
      while(tasks[i]->arrival_time > current_time){
        printf(" %2d        --\n", current_time);
        current_time++;
      }
      // reduce service time as the thread runs and increment current time
      while(tasks[i]->remaining_time > 0){
        printf(" %2d   %c%d  ", current_time, tasks[i]->task_id,tasks[i]->remaining_time);

        // find the ready list
        // NOTE: due to the structure of this project, if a task has not arrived
        //       yet, any tasks past it in the queue could not have arrived either
        rover = tasks[i]->next;
        ready_list=0; // keeps track of number of tasks on ready list, set to 0
        if(rover == NULL){
          printf(" --\n");
        } else{
          while(rover != NULL){
            if(rover->arrival_time <= current_time){
              if(ready_list == 0){
                printf(" %c%d", rover->task_id,rover->remaining_time);
              } else{
                printf(",%c%d", rover->task_id,rover->remaining_time);
              }
              // wait time increases on the ready list
              rover->wait_time++;
              ready_list++;
            } else{
              if(ready_list==0){
                printf(" --");
              }
              break; //prints extra -- without a break
            }
            rover = rover->next;
          }
          printf("\n");
        }
        tasks[i]->remaining_time--;
        current_time++;
      }
      tasks[i]->completion_time = current_time;
      tasks[i]->response_time = tasks[i]->completion_time-tasks[i]->arrival_time;
    }
  } else if(sched == 1){ // SJF(preemptive) case handler
    printf("SJF(preemptive) scheduling results\n\n");
    printf("time  cpu  ready queue (tid/rst)\n");
    printf("----  ---  ---------------------\n");
    //for(i=0;i<=task_counter;i++){
    remaining_tasks = task_counter+1;
    while(remaining_tasks > 0){
      // while no thread is running due to not arriving yet, increment time
      running = 0;
      for(i=0;i<task_counter;i++){
        if(tasks[i]->remaining_time > 0 && tasks[i]->arrival_time <= current_time){
          running++;
        }
      }
      if(running == 0){
        printf(" %2d        --\n", current_time);
        current_time++;
      } else{

        // find task with minimum remaining time and run that task for a time increment
        min_serv = 0;
        for(j=0;j<=task_counter;j++){
          // set min_serv as max service time to find next min service time
          if(min_serv == 0 && tasks[j]->remaining_time > min_serv){
            min_serv = tasks[j]->remaining_time + 1;
          }
          // if thread has the least time left to run and is not completed
          if(tasks[j]->arrival_time <= current_time &&  // task has arrived
             tasks[j]->remaining_time < min_serv &&     // task has the shortest time left to run
             tasks[j]->remaining_time > 0){             // task has remaining time
            min_serv = tasks[j]->remaining_time;
            thread_choice = j;
          }
        }

       // reduce service time as the thread runs and increment current time
        printf(" %2d   %c%d  ", current_time, tasks[thread_choice]->task_id,tasks[thread_choice]->remaining_time);
        ready_list=0;

        // find ready list
        for(j=0;j<=task_counter;j++){
          if(tasks[j]->arrival_time <= current_time &&   // task has arrived
             tasks[j]->remaining_time > 0 &&             // task still needs to run
             tasks[j]->task_id != 'A'+(thread_choice)){  // not the task running

            if(ready_list == 0){ // check if first item in ready list for format
              printf(" %c%d", tasks[j]->task_id,tasks[j]->remaining_time);
              // add wait time and number of ready list items if task must wait
              ready_list++;
              tasks[j]->wait_time++;

            } else{
              printf(",%c%d", tasks[j]->task_id,tasks[j]->remaining_time);
              // add wait time if task must wait
              tasks[j]->wait_time++;
            }
          }
          //wait time increases on the ready list
          if(ready_list==0 && j==task_counter){
            printf(" --");
          }
        }
        printf("\n");
        tasks[thread_choice]->remaining_time--;
        current_time++;

        if(tasks[thread_choice]->remaining_time == 0){
          tasks[thread_choice]->completion_time = current_time;
          tasks[thread_choice]->response_time = tasks[thread_choice]->completion_time-tasks[thread_choice]->arrival_time;
          remaining_tasks--;
        }
      }
    }
  } else if(sched == 2){
    printf("RR scheduling results (time slie is 1)\n\n");
    printf("time  cpu  ready queue (tid/rst)\n");
    printf("----  ---  ---------------------\n");

    remaining_tasks = task_counter+1; // +1 since zero-indexed
    rover = tasks[task_counter]; // first task will always be A based on specifications
    while(remaining_tasks > 0){
      // check if any tasks are running, wait if not
      running = 0;
      for(i=0;i<=task_counter;i++){
        if(tasks[i]->remaining_time > 0 && tasks[i]->arrival_time <= current_time){
          running = 1;
        }
      }
      if(running == 0){ //if should wait, wait
        printf(" %2d        --\n", current_time);
        current_time++;
      } else{ // otherwise run for a time slice
        // find next task to run
        found_next_task = 0;
        while(found_next_task == 0){
          rover = rover->next;
          if(rover == NULL){
            rover = tasks[0];
          }
          if(rover->remaining_time > 0 && rover->arrival_time <= current_time){
            found_next_task = 1;
          }
        }

        // print whatever task is next to run
        printf(" %2d   %c%d   ", current_time, rover->task_id,rover->remaining_time);
        rover->remaining_time--;

        // if task is complete decrement remaining tasks
        if(rover->remaining_time == 0){
          remaining_tasks--;
          rover->completion_time = current_time;
          rover->response_time = rover->completion_time-rover->arrival_time;
        }
        // find ready list
        ready_list=0;
        temp = rover->next;
        if(temp == NULL){
          temp = tasks[0];
        }
        while(temp!=rover){
          if(temp->remaining_time > 0 &&            // there is remaining run time
             temp->arrival_time <= current_time){   // task is running
            if(ready_list == 0){
              printf("%c%d", temp->task_id,temp->remaining_time);
            } else{
              printf(",%c%d", temp->task_id,temp->remaining_time);
            }
            temp->wait_time++;
            ready_list++;
          }
          temp=temp->next;
          if(temp == NULL){
            temp = tasks[0];
          }
        }
        if(ready_list == 0){
          printf("--\n");
        } else{
          printf("\n");
        }
        current_time++;
      }
    }
  } else{
    // the only way this error should occur is if the sched variable is changed
    // between read-in and running this if-else statement, which should be
    // (in theory) impossible given the operational bounds of this program
    printf("A horrible error has occured, this should not happen!!\n");
    exit(0);
  }

  // printf("time cpu ready queue (tid/rst)\n---- --- ---------------------\n");
    // print out statistics
  printf("\n     arrival service completion response wait");
  printf("\ntid   time    time      time      time   time");
  printf("\n--- -------- ------- ---------- -------- ----\n");

  // use a rover to print out all stats for the scheduling method
  rover = tasks[0];
  while(rover != NULL){
    printf(" %c     %2d      %2d       %2d        %2d     %2d\n",
            rover->task_id,rover->arrival_time,rover->service_time,
            rover->completion_time,rover->response_time,rover->wait_time);

    rover = rover->next;
  }

  // print final stats of service time and wait Ar_time
  printf("\nservice wait");
  printf("\n time   time");
  printf("\n------- ----\n");

  j = 0;
  //find max service time to print out in descending order
  for(i=0;i<task_counter;i++){
    if(tasks[i]->service_time >= j){
      j = tasks[i]->service_time;
    }
  }
  // print service time and associated wait time in increasing order
  while(task_counter >= 0){
    // reset max service time each run
    max_serv = j;
    rover = tasks[0];
    // find lowest unprinted service time and print for each task
    while(rover != NULL){
      if(rover->service_time <= max_serv && rover->printed != 1){
        max_serv = rover->service_time;
        i = (int)(rover->task_id - 'A');
      }
      rover=rover->next;
    }
    printf("  %2d     %2d\n", tasks[i]->service_time, tasks[i]->wait_time);
    tasks[i]->printed = 1;
    task_counter--;
  }

  // free all tasks, used and unused
  for(i=0;i<MAX_TASKS;i++){
    free(tasks[i]);
  }
  // free head pointer
  free(*tasks);
  return 0;
}


// printf("SJF(preemptive) scheduling results\n\n");
// printf("time  cpu  ready queue (tid/rst)\n");
// printf("----  ---  ---------------------\n");
// //for(i=0;i<=task_counter;i++){
// remaining_tasks = task_counter+1;
// while(remaining_tasks > 0){
//   // while no thread is running due to not arriving yet, increment time
//   running = 0;
//   for(i=0;i<task_counter;i++){
//     if(tasks[i]->remaining_time > 0 && tasks[i]->arrival_time <= current_time){
//       running++;
//     }
//   }
//   if(running == 0){
//     printf(" %2d        --\n", current_time);
//     current_time++;
//   } else{
//
//     // find task with minimum remaining time and run that task for a time increment
//     min_serv = 0;
//     for(j=0;j<=task_counter;j++){
//       // set min_serv as max service time to find next min service time
//       if(min_serv == 0 && tasks[j]->remaining_time > min_serv){
//         min_serv = tasks[j]->remaining_time;
//       }
//       // if thread has the least time left to run and is not completed
//       if(tasks[j]->arrival_time <= current_time && // task has arrived
//          tasks[j]->remaining_time <= min_serv &&   // task has the shortest time left to run
//          tasks[j]->remaining_time > 0){            // task has remaining time
//         min_serv = tasks[j]->remaining_time;
//         thread_choice = j;
//       }
//     }

//    // reduce service time as the thread runs and increment current time
//     while(tasks[thread_choice]->remaining_time > 0){
//       printf(" %2d   %c%d  ", current_time, tasks[thread_choice]->task_id,tasks[thread_choice]->remaining_time);
//       ready_list=0;
//
//       for(j=0;j<=task_counter;j++){
//       // rover = tasks[0];
//       // while(rover!= NULL){
//         if(tasks[j]->arrival_time <= current_time &&   // task has arrived
//            tasks[j]->remaining_time > 0 &&             // task still needs to run
//            tasks[j]->task_id != 'A'+(thread_choice)){  // not the task running
//
//           if(ready_list == 0){ // check if first item in ready list for format
//             printf(" %c%d", tasks[j]->task_id,tasks[j]->remaining_time);
//             // add wait time and number of ready list items if task must wait
//             ready_list++;
//             tasks[j]->wait_time++;
//
//           } else{
//             printf(",%c%d", tasks[j]->task_id,tasks[j]->remaining_time);
//             // add wait time if task must wait
//             tasks[j]->wait_time++;
//           }
//         }
//           //wait time increases on the ready list
//           if(ready_list==0 && j==task_counter){
//             printf(" --");
//           }
//       }
//       printf("\n");
//       tasks[thread_choice]->remaining_time--;
//       current_time++;
//     }
//     tasks[thread_choice]->completion_time = current_time;
//     tasks[thread_choice]->response_time = tasks[thread_choice]->completion_time-tasks[thread_choice]->arrival_time;
//   }
// }
