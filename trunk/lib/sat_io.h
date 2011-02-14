/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * This file defines procedures of IO for reading a SAT instance in the
 * @see <a href="http://logic.pdmi.ras.ru/~basolver/dimacs.html">DIMACS</a>
 * format, and initializes a @e sat_status variable.
 *
 * Other procedures for printing a solution or current status of the SAT
 * instance are also defined.
 * 
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/

#ifndef _KECOSATS_SAT_IO_
#define _KECOSATS_SAT_IO_

#include<stdio.h>
#include<stdlib.h>
#include"sat.h"

/**
 * The purpose of this procedure is to initialize a clause from an array
 * of integers which represent the literals occurring in the aforementioned
 * clause.
 *
 * Note: It may occur that lit is an array with a length larger than the
 *       number of literals.
 *
 * @param cl A pointer to an unitialized clause.
 * @param clause_length The number of literals that occur in the clause
 *        pointed to by cl.
 * @param lit An integer array with numbers which represent the literals
 *        that occur in the single clause pointed to by cl.
 *
 */
void set_clause( clause* cl, int clause_length, int lit[] );

/**
 * This function allocates memory for the elements that constitute
 * the global structure 'sat_st'.
 *
*/
void allocate_sat_status();

/**
 * This function parses a file that contains -- in DIMACS format--
 * the boolean formula that we intend to solve. It then allocates
 * space for the global structure sat_st and initializes its elements
 * with the information just collected from the file.
 *
 * @see  DIMACS reference...
 */
void set_initial_sat_status();

void print_formula();

void print_status();

void print_sol(int status);

#endif
