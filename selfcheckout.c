/////////////////////////////////////////////////////////////////////////////////////////////
//
// This code represents the simulation. Some of this code is taken and/or adapated from
// a simulation written by Dr. Richard Fujimoto at the Georgia Institute of Technology.
//
/////////////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "engine.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Execution time statistics (for measuring runtime of program); unrelated to sim model //
//////////////////////////////////////////////////////////////////////////////////////////

clock_t StartTime, EndTime;     // start and end time of simulation run
int NumberOfEvents=0;           // number of events executed


///////////////////////
//  State Variables  //
///////////////////////

// Simulation constants; all times in minutes
// A = mean interarrival time (drawn from exponential distribution)
// C = time for each item to be scanned, averaged to include the payment processing
#define A   4.0
#define C   0.33

// Number of customers to be simulated (used to determine length of simulation run)
#define NARRIVALS   10

// Flag set to 1 to print debugging statements (event trace), 0 otherwise
#define DB  1

// State Variables of Simulation
int InTheCheckout=0; // number of customers waiting to use the checkout kiosk, or using it
int KioskFree=1;     // boolean: 1 if kiosk is free, 0 otherwise
int ArrivalCount=0;  // number of arrivals simulated; used for termination

// State variables used for statistics
double  TotalWaitingTime = 0.0; // total time waiting to checkout
double  LastEventTime = 0.0;    // time of last event processed; used to compute TotalWaitingTime


/////////////////////////////////
// Data structures for events  //
/////////////////////////////////

// types of events
typedef enum {ARRIVAL, CHECKOUT} KindsOfEvents;

// Event parameters
// No event parameters really needed in this simple simulation
struct EventData {
    KindsOfEvents EventType;
};



/////////////////////////////////
//     Function Prototypes     //
/////////////////////////////////

// prototypes for event handlers
void Arrival (struct EventData *e);     // customer arrival event
void Checkout (struct EventData *e);    // customer checkout event

// prototypes for other procedures
double RandExp(double M);           // random variable, exponential distribution
double CalcNumOfItems(void);    // random variable [0,20]

//////////////////////////////////
//    Random Number Generator   //
//////////////////////////////////

// Compute exponenitally distributed random number with mean M
double RandExp(double M)
{
    double urand;   // uniformly distributed random number [0,1)
    urand = ((double) rand ()) / (((double) RAND_MAX)+1.0); // avoid value 1.0
    return (-M * log(1.0-urand));
}

double CalcNumOfItems(void) {
    double N;              // number of items a customer has
    N = rand() % 20 + 1;   // random integer between 1 and 20
    return N; 
}


////////////////////////////////////////////////////////////////
//    Event Handlers                                          //
//    Parameter is a pointer to the data portion of the event //
////////////////////////////////////////////////////////////////

//event handler for customers arriving at the checkout
void Arrival (struct EventData *e)
{
    struct EventData *d;
    double ts;

    // catch non-arrival event types
    if (e->EventType != ARRIVAL) {fprintf(stderr, "Unpexcted event type\n"); exit(1);}

    if (DB) printf("Arrival Event: time=%f\n", CurrentTime());

    // update waiting time statistics
    if (InTheCheckout > 1) { // if there are customers waiting, update total waiting time
        TotalWaitingTime += ((InTheCheckout-1) * (CurrentTime()-LastEventTime));
    }

    // update the event count and the number of customers in line
    NumberOfEvents++;
    InTheCheckout++;

    // schedule the next arrival event if it's not the last of the arrivals
    ArrivalCount++;
    if (ArrivalCount < NARRIVALS) {
        if ((d=malloc(sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = ARRIVAL;
        ts = CurrentTime() + RandExp(A);
        Schedule (ts, d, (void *) Arrival);
    }
    
    if (KioskFree) {
        //Kiosk no longer free since we'll be scheduling a checkout event
        KioskFree = 0;

        //schedule the checkout event
        if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = CHECKOUT;
        ts = CurrentTime() + C * CalcNumOfItems();
        Schedule  (ts, d, (void *) Checkout);
    }

    LastEventTime = CurrentTime(); // time of the last event processed
    free (e);                      // free storage for event parameters
};

// event handler for checkout events
void Checkout (struct EventData *e)
{
    struct EventData *d;
    double ts;

    if (e->EventType != CHECKOUT) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
    if (DB) printf ("Checkout Event: time=%f\n", CurrentTime());

    // update waiting time statistics
    if (InTheCheckout > 1) { // if there are customers waiting, update total waiting time
        TotalWaitingTime += ((InTheCheckout-1) * (CurrentTime()-LastEventTime));
    }

    // update the event count and number of customers in checkout line
    NumberOfEvents++;
    InTheCheckout--;

    // schedule checkout event
    if (InTheCheckout>0) {
        //  schedule checkout event for the next customer
        if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = CHECKOUT;
        printf("Num items times time: %f\n", CalcNumOfItems());
        ts = CurrentTime() + C * CalcNumOfItems();
        Schedule (ts, d, (void *) Checkout);
    }
    else {
        // checkout kiosk is now free
        KioskFree = 1;
    }

    LastEventTime = CurrentTime();      // time of last event processed
    free (e);                           // event parameters
}

int main (void)
{
    struct EventData *d;
    double ts;
    double Duration;

    // initialize event list with the first customer arrival
    if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = ARRIVAL;
    ts = RandExp(A);
    Schedule (ts, d, (void *) Arrival);

    printf ("Welcome to the Self-Checkout Kiosk Simulation\n");
    StartTime = clock();
    RunSim();
    EndTime = clock();

    // print final statistics
    printf ("Number of customers = %d\n", NARRIVALS);
    printf ("Total waiting time = %f\n", TotalWaitingTime);
    printf ("Average waiting time = %f\n", TotalWaitingTime / (double) NARRIVALS);

    Duration = (double) (EndTime-StartTime) / (double) CLOCKS_PER_SEC;
    printf ("%d events executed in %f seconds (%f events per second)\n", NumberOfEvents, Duration, (double)NumberOfEvents/Duration);

    return 0;
}

