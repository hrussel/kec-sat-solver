
#include "kecosats_structures.h"
#include "clause_learning.h"
#include <string.h>

#define max(a,b) (a<b?b:a)

/** 
 *
 *  This function returns the backtracking jumping destiny that will be
 *  reached as a consecuence of analyse this conflict.
 *  
 *  This level is determined by the maximum decision level of the literals
 *  that are in the conflict_clause.
 *
 *  @param clause_length The number of literals in conflict_clause
 *  @param conflict_clause The conflictive clause that will be learned.
 */
int get_bt_target_level( int clause_length, int* conflict_clause );

/**
 *
 * This function analyses a conflict and returns the decision_level to which
 * the search should backtrack and proceed.
 *
 * If the decision_level equal to 0, it indicates that there's no escape, the
 * conflict is inevitable and thus, the formula is unsatisfiable.
 *
 * @return The decision_level
 *
 */
int analyze_conflict();

/**
 *
 * This function analyses the conflict and determines the conflictive clause
 * that will be learned.
 *
 * @param abs_literal The variable that has two assigned values.
 * @param conflict_cl_length The length of the learned clause:
 * @return An array of the literals that ocurrs in the learned clause with
 *         its polarity.
 *
 */
int* get_conflict_induced_cl( variable abs_literal, int* conflict_cl_length );
