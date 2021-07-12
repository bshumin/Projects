/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#include "sim.h"
#include "event.h"
#include "queue.h"
#include "time.h"

#define QSZ 100 /* not used by queue.c */

int MAX_PASS = 100;
int MAX_SCAN = 1;
static void parse_args(int argc, char** argv);



int main(int argc, char **argv)
{
  /* process command line arguments */
  parse_args(argc, argv);


  int i;
  int min_scanQ;
  event_t *temp_ev;
  event_t *new_ev;
  event_t *start_ev;
  int num_passengers = 0; /* counts the number of passengers */

  int atAirlineDesk = 0;
  int atIDDesk = 0;
  int atScanDesk[MAX_SCAN];
  int atTrainDesk = 0;

  queue_t *airlineQ;
  queue_t *idQ;
  queue_t *scanQ[MAX_SCAN];
  queue_t *trainQ;



    /* initialize modules */
    event_init(QSZ);
    time_init();


    start_ev = event_create();
    start_ev->passenger = passenger_create();
    start_ev->passenger->pass_id = ++num_passengers;
    start_ev->event_time = 0.0;
    start_ev->event_type = EV_ARRIVE;
    event_schedule(start_ev);

    airlineQ = queue_init(QSZ);
    idQ = queue_init(QSZ);
    trainQ = queue_init(QSZ);
    for(i=0;i<MAX_SCAN;i++){
      scanQ[i] = queue_init(QSZ);
      atScanDesk[i] = 0;
    }

    /* run main loop */
    while(!event_empty())
    {

        new_ev = event_cause();
        //time_set(new_ev->event_time);
        switch (new_ev->event_type)
        {
        case (EV_ARRIVE) :
        {

            printf("new passenger %d arrives: %f\n",
                   new_ev->passenger->pass_id,
                   new_ev->event_time);

            temp_ev = event_create();
            temp_ev->passenger = new_ev->passenger;
            temp_ev->passenger->arrival_time = time_get();
            temp_ev->event_time = time_airlineQ();
            temp_ev->event_type = EV_AIRLINEQ;
            event_schedule(temp_ev);
            if (MAX_PASS > num_passengers++)
            {

                temp_ev = event_create();
                temp_ev->passenger = passenger_create();
                temp_ev->passenger->pass_id = num_passengers;
                temp_ev->event_time = time_arrive();
                temp_ev->event_type = EV_ARRIVE;
                event_schedule(temp_ev);
            }
        }
            break;
        case (EV_AIRLINEQ) :

            printf("passenger %d arrives in airline queue: %f\n",
                   new_ev->passenger->pass_id,
                   new_ev->event_time);
            new_ev->passenger->airlineQ_time = time_get();

          if((atAirlineDesk == 1) ){
             queue_insert(airlineQ, new_ev->passenger);
          }

          else if(atAirlineDesk == 0){

            temp_ev = event_create();
            temp_ev->passenger = new_ev->passenger;
            temp_ev->event_time = time_airline();
            temp_ev->passenger->airline_time = time_get();
            temp_ev->event_type = EV_AIRLINE;
            event_schedule(temp_ev);
            atAirlineDesk = 1;
          }
          break;

        case (EV_AIRLINE) :
          printf("new passenger %d leaves airline desk: %f\n",
               new_ev->passenger->pass_id,
               new_ev->event_time);

          temp_ev = event_create();
          temp_ev->passenger = new_ev->passenger;
          temp_ev->event_time = time_idQ();
          temp_ev->event_type = EV_IDQ;
          event_schedule(temp_ev);
          atAirlineDesk = 0;

          //grab next passenger from queue
          if(queue_peek(airlineQ)!=NULL){

            temp_ev = event_create();
            temp_ev->passenger = queue_remove(airlineQ);
            temp_ev->event_time = time_airline();
            temp_ev->passenger->airline_time = time_get();
            temp_ev->event_type = EV_AIRLINE;
            event_schedule(temp_ev);
            atAirlineDesk = 1;
          }

            break;
        case (EV_IDQ) :
          printf("passenger %d enters ID queue: %f\n",
                 new_ev->passenger->pass_id,
                 new_ev->event_time);
          new_ev->passenger->idQ_time = time_get();

          if((atIDDesk == 1) ){
            queue_insert(idQ, new_ev->passenger);
          }

          else if(atIDDesk == 0){

            temp_ev = event_create();
            temp_ev->passenger = new_ev->passenger;
            temp_ev->event_time = time_id();
            temp_ev->passenger->id_time = time_get();
            temp_ev->event_type = EV_ID;
            event_schedule(temp_ev);
            atIDDesk = 1;
        }
            break;
        case (EV_ID) :
          printf("new passenger %d leaves at ID desk: %f\n",
               new_ev->passenger->pass_id,
               new_ev->event_time);

          temp_ev = event_create();
          temp_ev->passenger = new_ev->passenger;
          temp_ev->event_time = time_scanQ();
          temp_ev->event_type = EV_SCANQ;
          event_schedule(temp_ev);
          atIDDesk = 0;
          //grab next person from queue
          if(queue_peek(idQ)!=NULL){
            temp_ev = event_create();
            temp_ev->passenger = queue_remove(idQ);
            temp_ev->passenger->id_time = time_get();
            temp_ev->event_time = time_id();
            temp_ev->event_type = EV_ID;
            event_schedule(temp_ev);
            atIDDesk = 1;
          }
            break;
        case (EV_SCANQ) :
          printf("passenger %d arrives in scan queue: %f\n",
                 new_ev->passenger->pass_id,
                 new_ev->event_time);
          temp_ev->passenger->scanQ_time = time_get();
          // check if any scanners are open, if not find smallest scan queue and
          // store value for passenger to enter
	  min_scanQ = queue_size(scanQ[0]);
	  for(i=0;i<MAX_SCAN;i++){

            if(atScanDesk[i] == 0){
              printf("passenger %d arrives at scanner %d: %f\n",
                     new_ev->passenger->pass_id,
                     i,
                     new_ev->event_time);

              temp_ev = event_create();
              temp_ev->passenger = new_ev->passenger;
              temp_ev->passenger->scan_num = i;
              temp_ev->passenger->scan_time = time_get();
              temp_ev->event_time = time_scan();
              temp_ev->event_type = EV_SCAN;
              event_schedule(temp_ev);
              atScanDesk[i] = 1;
              break; //break out of for loop if an empty queue is found
            } else if(queue_size(scanQ[i]) <= min_scanQ) {
              min_scanQ = i;
            }

            if(i == MAX_SCAN - 1){
              queue_insert(scanQ[min_scanQ], new_ev->passenger);
            }
          }
            break;
        case (EV_SCAN) :
          printf("new passenger %d arrives at train: %f\n",
               new_ev->passenger->pass_id,
               new_ev->event_time);

          temp_ev = event_create();
          temp_ev->passenger = new_ev->passenger;
          temp_ev->event_time = time_trainQ();
          temp_ev->event_type = EV_TRAINQ;
          event_schedule(temp_ev);
          atScanDesk[new_ev->passenger->scan_num] = 0;

          //grab next person from queue
          if(queue_peek(scanQ[new_ev->passenger->scan_num])!=NULL){
            temp_ev = event_create();
            temp_ev->passenger = queue_remove(scanQ[new_ev->passenger->scan_num]);
            temp_ev->passenger->scan_time = time_get();
            temp_ev->event_time = time_scan();
            temp_ev->event_type = EV_SCAN;
            event_schedule(temp_ev);
            atScanDesk[new_ev->passenger->scan_num] = 1;
          }
            break;
        case (EV_TRAINQ) :
          printf("passenger %d arrives in train queue: %f\n",
                 new_ev->passenger->pass_id,
                 new_ev->event_time);
          temp_ev->passenger->trainQ_time = time_get();

          if((atTrainDesk == 1) ){
            queue_insert(trainQ, new_ev->passenger);
          }

          else if(atTrainDesk == 0){

            temp_ev = event_create();
            temp_ev->passenger = new_ev->passenger;
            temp_ev->passenger->train_time = time_get();
            temp_ev->event_time = time_train();
            temp_ev->event_type = EV_TRAIN;
            event_schedule(temp_ev);
            atTrainDesk = 1;
          }
            break;
        case (EV_TRAIN) :
          printf("new passenger %d arrives at train: %f\n",
               new_ev->passenger->pass_id,
               new_ev->event_time);

          temp_ev = event_create();
          temp_ev->passenger = new_ev->passenger;
          temp_ev->event_time = time_gate();
          temp_ev->event_type = EV_GATE;
          event_schedule(temp_ev);
          atTrainDesk = 0;

        //grab next person from queue
          if(queue_peek(trainQ)!=NULL){
            temp_ev = event_create();
            temp_ev->passenger = queue_remove(trainQ);
            temp_ev->passenger->train_time = time_get();
            temp_ev->event_time = time_train();
            temp_ev->event_type = EV_TRAIN;
            event_schedule(temp_ev);
            atTrainDesk = 1;
          }
            break;
        case (EV_GATE) :
          printf("---passenger %d departs at gate: %f---\n",
               new_ev->passenger->pass_id,
               new_ev->event_time);
          new_ev->passenger->gate_time = time_get();
          passenger_destroy(new_ev->passenger);
            break;

        default :
            /* error */
            break;
        }
        /* free event */
        event_destroy(new_ev);
    }
    /* Print overall stats */
    /* finalize modules */
    queue_finalize(airlineQ);
    queue_finalize(idQ);
    queue_finalize(trainQ);
    for(i=0;i<MAX_SCAN;i++)
      queue_finalize(scanQ[i]);
    event_finalize();
    time_finalize();
    return 0;
}


// looko for -p for pass and -s for scan
static void parse_args(int argc,char **argv)
{
  int opt;

  while((opt = getopt(argc, argv,":tp:s:"))!=-1)
    {
      
      switch(opt){
      case 'p':
	MAX_PASS = atoi(optarg);
	printf("MAX_PASS = %i \n",MAX_PASS);
	break;
      case 's':
	MAX_SCAN = atoi(optarg);
	printf("MAX_SCAN = %i \n",MAX_SCAN);
	break;
      case '?':
	break;
      }
    }
}
  
 

