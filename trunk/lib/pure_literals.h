/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Definition for the procedures that detect pure literals in clauses and
 * assign them with values so as to simplify the SAT formula.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/
#ifndef _KECOSATS_PURE_LITERALS_
#define _KECOSATS_PURE_LITERALS_

#include <stdio.h>
#include "sat.h"

variable* find_pure_literals( int *pure_literals_size );

void swap_literals( clause* cl, int old_index, int new_index );

void swap_clauses( clause* formula, int old_index, int new_index );

int is_pure_literal( variable literal, variable* pure_literals,
                     int num_pure_literals );

#endif
