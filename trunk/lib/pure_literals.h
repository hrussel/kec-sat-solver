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

/**
 * This function finds all pure literals in a boolean formula.
 *
 * @pre (%forall i:0<=i<sat.st.num_vars: pure_literals[i].pure_literal =UNKNOWN);
 * @param pure_literals_size A pointer that will end up having the number
 *        of pure_literals in the formula
 * @return A pointer to an array of those variables (with their polarities) that
 *         are pure literals.
 */
variable* find_pure_literals( int *pure_literals_size );

/**
 * Given a clause pointer, it swaps two literals.
 *
 * @param cl A pointer to a clause whose literals are to be swapped.
 * @param old_index The index of one of the literals to be swapped.
 * @param new_index The index of the other literal that is going to be swapped.
 *
 */
void swap_literals( clause* cl, int old_index, int new_index );

/**
 * Given an array of clauses (or equivalently a formula) pointer, it swaps two
 * clauses of this formula. 
 *
 * @param formula A pointer to a formula whose clauses are going  to be swapped.
 * @param old_index The index of one of the clauses to be swapped.
 * @param new_index The index of the other clause that is going to be swapped.
 *
 */
void swap_clauses( clause* formula, int old_index, int new_index );

/**
 * This function determines if the literal @e literal is an element of
 * the array @e pure_literals.
 *
 * @param literal The variable to be determined if belongs to the array
 *        pure_literals.
 * @param pure_literals The array of variables.
 * @param num_pure_literals The size of the array pure_literals.
 * @return TRUE iff literal is contained in the array pure_literals.
 *
 */
int is_pure_literal( variable literal, variable* pure_literals,
                     int num_pure_literals );

/**
 *
 * This function eliminates all the literals contained in the array
 * pure_literals from a formula.
 * Note: One minor inconvenience with this function is that it
 *       alters the order of the literals in a clause and the order
 *       of appearance of the clauses in a formula.
 *
 * @param pure_literals An array of variables known to be pure literals.
 * @param num_pure_literals The number of pure literals in the array.
 * @return SATISFIABLE if there formula is found to be satisfied by assigning
 *         pure literals.
 *         DONT_CARE in any other case.
 *
 */
int  eliminate_pure_literals( variable* pure_literals, 
                              int num_pure_literals );
#endif
