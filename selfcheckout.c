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

clock_t StartTime, EndTime;	    // start and end time of simulation run
int NumberOfEvents=0;			// number of events executed


///////////////////////
//  State Variables  //
///////////////////////

// Simulation constants; all times in minutes
// A = mean interarrival time (drawn from exponential distribution)
// K = time using runway when landing
#define	A	2.0
#define	K	0.0

// Number of customers to be simulated (used to determine length of simulation run)
#define	NARRIVALS	20

// Flag set to 1 to print debugging statements (event trace), 0 otherwise
#define DB	1

// State Variables of Simulation
int	InTheCheckout=0; // number of customers waiting to use the checkout kiosk, or using it
int	RunwayFree=1;	 // boolean: 1 if kiosk is free, 0 otherwise
int	ArrivalCount=0;	 // number of arrivals simulated; used for termination

// State variables used for statistics
double	TotalWaitingTime = 0.0;	// total time waiting to checkout
double	LastEventTime = 0.0;	// time of last event processed; used to compute TotalWaitingTime


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
void Arrival (struct EventData *e);		// customer arrival event
void Checkout (struct EventData *e);	// customer checkout event

// prototypes for other procedures
double RandExp(double M);			// random variable, exponential distribution
  

//////////////////////////////////
//    Random Number Generator   //
//////////////////////////////////

// Compute exponenitally distributed random number with mean M
double RandExp(double M)
{
	double urand;	// uniformly distributed random number [0,1)
	urand = ((double) rand ()) / (((double) RAND_MAX)+1.0);	// avoid value 1.0
	return (-M * log(1.0-urand));
}