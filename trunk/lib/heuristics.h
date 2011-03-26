#include "sat_io.h"
#include "kecosats_structures.h"

/**
* This function returns, as branching variable, the first and still non-assigned
* variable. 
* 
* @return The variable over which the branching will be performed, along with 
*         its polarity.
*/
int decide_greedy();

/**
* This function returns, as branching variable, the one selected by berkmin's
* heuristic.
*
* @return The variable over which the branching will be performed, along with 
*         its polarity.
*/
int decide_berkmin();

/**
* This function returns the branching variable. This function constitutes a 
* slight modification of berkmin's heuristic.
* heuristic.
*
* @return The variable over which the branching will be performed, along with 
*         its polarity.
*/
int decide_kecosats();

/**
* The purpose of this function is to initialize the images ac(lit) and lit_ac()
*
*/
void initialize_kecosats_heuristic();
