/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Definition of the structure used for the SAT solver and signatures of the
 * procedures.
 *
 * The structure used is an array of pointers to clauses, where a clause is
 * an array of literals, and each literal is an integer. Clause watchers are
 * also included, and a stack for performing the non-chronological bactracking.
 *
 * The procedures defined in this file are used to operate and change the SAT
 * structure
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/
#ifndef _KECOSATS_ALGORITHM_
#define _KECOSATS_ALGORITHM_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "kecosats_structures.h"
#include "pure_literals.h"

/**
 * This function destroys an element of type decision_level_data:
 * It destroys its internal lists and also frees its own block of
 * data
 */
void free_decision_level_data(decision_level_data* dld);

void allocate_sat_status();

/**
 *
 * This function receives a @e literal and performs Boolean Constrain
 * Propagation (BCP) by calling the appropriate functions that perform Unit
 * Clause Propagation and Pure Literal Elimination.
 *
 * @param literal The variable selected for assignment by the function
 *        @e decide_next_branch.
 * @return Returns a status that indicates how the assignment to variable
 *        @e literal went on. It will return SATISFIED, if after assigning
 *        @e literal a truth value and after Unit Clause propagation and
 *        pure literal elimination has been performed, the formula was
 *        found to be satisfied. It will return DONT_CARE(0), if after assigning
 *        @e literal a truth value and after Unit Clause propagation and
 *        pure literal elimination has been performed, the formula hasn't
 *        still been satisfied and there is still no conflict, so there's the
 *        chance that if we continue to assign some other variables we may find
 *        the formula to be satisfied or conflicted. It will return CONFLICT(2),
 *        if the assignment of @e literal with some value, along with the values
 *        that were previously assigned to some variables is conflictive.
 *        
 */
int deduce(variable literal);

int set_newly_watchers( list* clauses_affected, variable literal );

/**
 * This function receives a stack of unitary clauses whose single variables need
 * to be properly assigned and propagated.
 *    
 * @param unit_clauses A stack of unitary clauses whose single variables need to
 *        be properly assigned and propagated.
 * @return UNIT_CLAUSE(1) If the clause head_clause is unitary.
 *         CONFLICT(2)    If the clause is conflictive with the current model.
 *         DONT_CARE(0)   Neither of the previous two alternatives.
 * @pre unit_clauses != NULL;
 */

int unit_propagation( stack* clauses_not_made_true);

void unassign(variable literal);

/**
 * Decide the next variable for the backtracking to continue,
 * once chosen, push it to the @e backtracking_status stack of the
 * sat_st global variable as a @e decision_level_data structure.
 *
 * The policy for chosing an unset variable is to select the one that has
 * the biggest number of watchers. So as to maximize the number
 * of satisfied clauses in the assingment and detect faster conflicts
 * or satisfability of the formula. Thereby, the time complexity of this
 * procedure is O(num_vars).
 */
int decide_next_branch();


//int sove_sat();

/**
 * This procedure is used for backtracking purposes.
 * The procedure undoes all the assignments made in the
 * given decision level.
 *
 * @param dec_lev_dat    The structure that describes the operations
 *                       made in the decision level
 */
void undo_assignments(decision_level_data *dec_lev_dat);


int preprocess();

/**
 * This function tries to solve the sat_instance that is stored
 * in the global variable sat_st.
 *
 * @return   SATISFIABLE if an assignment to the SAT variables is
 *           found that satisfies the formula. Otherwise, UNSATISFIABLE.
 */
int solve_sat();


#endif
