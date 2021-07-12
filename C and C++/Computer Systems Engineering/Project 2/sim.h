/* <sim.h> */

#ifndef SIM_H
#define SIM_H 1

/* The event types are as follows: */

#define EV_ARRIVE   0 /* this is when passenger arrives
                         at the airport */
#define EV_AIRLINEQ 1 /* this is when she selects an airline
                         and gets in line to check bags*/
#define EV_AIRLINE  2 /* this is when she gets to the agent
                         to check bags */
#define EV_IDQ      3 /* this is when she enters the line 
                         to check her ID */
#define EV_ID       4 /* this is when she gets her ID check */
#define EV_SCANQ    5 /* this is when she gets in line for the scan */
#define EV_SCAN     6 /* this is when she gets to the scanner */
#define EV_TRAINQ   7 /* this is when she is finished being
                         scanned and heads for the train */
#define EV_TRAIN    8 /* this is when she gets off the train
                         and heads for her gate */
#define EV_GATE     9 /* this is when she arrives at the gate */

typedef struct passenger_s passenger_t;
struct passenger_s
{
    int    pass_id;      /* passenger number */
    int    scan_num;     /* scanner visited by the passenger */
    double arrival_time; /* gets to the airport */
    double airlineQ_time;/* gets in first queue */
    double airline_time; /* gets to head of first queue*/
    double idQ_time;     /* gets in ID queue */
    double id_time;      /* gets to head of ID queue */
    double scanQ_time;   /* gets in scanner queue */
    double scan_time;    /* gets to head of scanner queue */
    double trainQ_time;  /* gets in train queue */
    double train_time;   /* gets to head of train queue */
    double gate_time;    /* gets to gate */
};

static inline passenger_t *passenger_create()
{
    passenger_t *new = (passenger_t *)malloc(sizeof(passenger_t));
    memset(new, 0, sizeof(passenger_t));
    return new;
}

static inline void passenger_destroy(passenger_t *old)
{
    free(old);
}

#endif
