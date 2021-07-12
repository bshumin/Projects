
#include "defs.h"
/* various time related functions */
static double sim_time;
//delta time
//current time
void time_init(void){
  sim_time = 0;
}

void time_fini(void){
  //not implemented in this stage
}

double time_get(void){
  return sim_time;
}

void time_set(double t){
  sim_time = t;
}

// returns arrival time
double time_arrive(){
  double temp = interarrival_time();
  return temp;
}

// blank
double time_airline(){
  double temp = enter_airline_queue_time();
  return temp;
}

// blank
double time_id(){
  double temp = 0;
return temp;
}

// blank
double time_scan(){
  return 0;
}

// blank
double time_train(){
  return 0;
}

// blank
double time_gate(){
  return 0;
}
