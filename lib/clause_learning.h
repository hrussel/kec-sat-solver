#include "sat_io.h"
#include "kecosats_structures.h"
#include "kecosats_algorithm.h"

/**
 *
 * Given an array of literals, it adds a new clause to the formula.
 * This function is inteded for use in the stage of learning a new
 * clause during the non-chronological backtracking.
 *
 * @param clause_length The number of literals in the array @e lit.
 * @param lit An array which contains the literals that constitute
 *        the clause.
 *
 */
clause* learn_clause( int clause_length, int lit[] );

/**
 *
 * Deletes the clause indexed by @e clause_index from the formula.
 * @param clause_index The index of the clause to be deleted.
 *
 *
 */
void unlearn_clause( int clause_index );

/**
 *
 * This function searches for all clauses marked @e too_large: those which have
 * more than @esat_st.clause_upper_bound literals. If one of these clauses is
 * found to be non-unitary, it will be unlearned (discarded). Otherwise, it will
 * be preserved.
 *
 */
void unlearn_large_clauses();
