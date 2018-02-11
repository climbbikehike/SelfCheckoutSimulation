/////////////////////////////////////////////////////////////////////////////////////////////
//
// This code represents the simulation engine. Some of this code is taken and/or adapated from
// a simulation written by Dr. Richard Fujimoto at the Georgia Institute of Technology.
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>    //standard library for input and output
#include <stdlib.h>   //standard library for utility functions
#include "engine.h"

// Data srtucture for an event; this is independent of the application domain
struct Event {
	double timestamp;		// event timestamp
	void *AppData;			// pointer to event parameters
	void (*callback)(void *);	// callback, parameter is event
	struct Event *Next;		// priority queue pointer
} ;

