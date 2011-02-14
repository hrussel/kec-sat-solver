/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Source code for the pure_literal operations.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/
#include "pure_literals.h"
#include <stdio.h>

typedef struct {
    int polarity;
    int is_pure_literal;
} pure_literal_status;


/**
 * This function finds all pure literals in a boolean formula.
 *
 * @pre (%forall i:0<=i<sat.st.num_vars: pure_literals[i].pure_literal =UNKNOWN);
 * @param pure_literals_size A pointer that will end up having the number
 *        of pure_literals in the formula
 * @return A pointer to an array of those variables (with their polarities) that
 *         are pure literals.
 */
variable* find_pure_literals( int* pure_literals_size ) {
    int i=0,j=0;
    variable literal;
    int abs_literal;
    clause current_clause;
    pure_literal_status pl_status[sat_st.num_vars+1];    
    variable* pure_literals =
      (variable*) malloc( (sat_st.num_vars+1)*sizeof(variable));
    int curr_literal_polarity;

    *pure_literals_size = 0;
    memset( pl_status, -1, (sat_st.num_vars+1)*sizeof(pure_literal_status));

    for ( i=0; i< sat_st.num_clauses; i++ ) {
        current_clause = sat_st.formula[i];
        //@pre current_clause is satisfied.
        //if ( current_clause.satisfied != TRUE ) {
            for ( j=0; j< current_clause.size; j++ ) {
                literal = current_clause.literals[j];
                abs_literal = abs( literal );

                // If current literal is known already to be an impure literal,
                // proceed.
                if ( pl_status[abs_literal].is_pure_literal == FALSE ) {
                    continue;
                }
                else if (pl_status[abs_literal].is_pure_literal == UNKNOWN){
                    curr_literal_polarity = abs_literal/literal;
                    pl_status[abs_literal].polarity =
                      curr_literal_polarity;
                    pl_status[abs_literal].is_pure_literal = TRUE;
                }
                else { 
                    // @Assert: pl_status[abs_literal].pure_literal == TRUE;
                    curr_literal_polarity = abs_literal/literal;
                    if ( pl_status[abs_literal].polarity !=
                         curr_literal_polarity ){
                      printf("%d no es puro \n", abs_literal);
                      pl_status[abs_literal].is_pure_literal = FALSE;
                    }
                }
            }
            //}
    }

    for ( i=1; i<= sat_st.num_vars; i++ ) {
        if ( pl_status[i].is_pure_literal == TRUE ) {
            pure_literals[*pure_literals_size] = i*pl_status[i].polarity;
            printf("%d es puro\n", i*pl_status[i].polarity);
            (*pure_literals_size)++;
        }
    }

    return pure_literals;
}

///**
// *
// * This function eliminates all the literals contained in the array
// * pure_literals from a formula.
// *
// * @param pure_literals An array of variables known to be pure literals.
// * @param num_pure_literals The number of pure literals in the array.
// * @return SATISFIABLE if there formula is found to be satisfied by assigning
// *         pure literals.
// *         DONT_CARE in 
// *         CONFLICT if there's a conflictive assignation of boolean values.
// *         to varaibles.
// *
// */
//
//int  eliminate_pure_literals( variable* pure_literals, 
//                              int pure_literals_size ) {
//    int i;
//    int status = DONT_CARE;
//
//    for( i=0; i< pure_literals_size; i++ ) {
//      // This should be a rewriting of the formula instead of
//      // a deduce.
//        status = deduce( pure_literals[i] );
//        if ( status != DONT_CARE ) {
//             return status;
//        }
//    }
//    free( pure_literals );
//    return status;
//}

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
                              int num_pure_literals ) {
    int i,j;
    int status = DONT_CARE;
    int abs_literal;
    variable literal;
    clause* current_clause;
    // For each pure literal, update the model to make them true.
    // For each pure literal, set its pos_watched_list and its
    // neg_watched_list to be empty lists.
    for( i=0; i<num_pure_literals; i++ ) {
        literal = pure_literals[i];
        abs_literal = abs( literal );

        sat_st.model[abs_literal] = (abs_literal/literal == 1 ? TRUE : FALSE);
        // Empty its pos_watched_list and its neg_watched_list.
        while( ! empty(&sat_st.pos_watched_list[abs_literal]) ) {
            pop( &sat_st.pos_watched_list[abs_literal] );
        }
        while( ! empty(&sat_st.neg_watched_list[abs_literal]) ) {
            pop( &sat_st.neg_watched_list[abs_literal] );
        }
    }

    for ( i=0; i< sat_st.num_clauses; i++ ) {
        current_clause = sat_st.formula + i;

        for ( j=0; j< current_clause->size; j++ ) {
            literal = current_clause->literals[j];
            abs_literal = abs( literal );

            if ( is_pure_literal(literal, pure_literals, num_pure_literals) ) {
                // If the clause is a unitary clause and it's single literal
                // is a pure one. Eliminate the clause.
                if ( current_clause->size == 1 ) {

                    if ( sat_st.num_clauses <= 1 ) {
                        return SATISFIABLE;
                    }
                    // If there's another clause of the formula still
                    // unexamined, it may be swapped with it.
                    if ( sat_st.num_clauses -i > 1 ) {
                        swap_clauses( sat_st.formula, i, sat_st.num_clauses-1 );
                    }
                    
                    sat_st.num_clauses--;
                    free( current_clause->literals );
                    // There's no need to update current_clauses' watchers,
                    // since it's now a nonexisting clause.
                }        
                else if ( current_clause->size > 1 ) {
                    // If there's another literal of the clause still
                    // unexamined, it may be swapped with it.
                    if ( current_clause->size -j> 1 ) {
                        swap_literals(current_clause, j,current_clause->size-1);
                    }
                    (current_clause->size)--;

                    // Update current_clause's watchers.
                    if ( is_head_watcher(current_clause, literal) ) {
                        status = update_watcher( current_clause );
                        if ( status == UNIT_CLAUSE ) {
                            current_clause->head_watcher =
                                current_clause->tail_watcher;
                        }
                    }
                    else if ( is_tail_watcher(current_clause, literal) ) {
                      swap_watchers( current_clause);
                      status = update_watcher( current_clause );
                        if ( status == UNIT_CLAUSE ) {
                            current_clause->head_watcher =
                                current_clause->tail_watcher;
                        }
                    }
                }
            }
        }
    }
    return status;
}

/**
 * Given a clause pointer, it swaps two literals.
 *
 * @param cl A pointer to a clause whose literals are to be swapped.
 * @param old_index The index of one of the literals to be swapped.
 * @param new_index The index of the other literal that is going to be swapped.
 *
 */
void swap_literals( clause* cl, int old_index, int new_index ) {
    variable temp_literal;
    temp_literal = (cl->literals)[old_index];
    (cl->literals)[old_index] = (cl->literals)[new_index];
    (cl->literals)[new_index] = temp_literal;
}

/**
 * Given an array of clauses (or equivalently a formula) pointer, it swaps two
 * clauses of this formula. 
 *
 * @param formula A pointer to a formula whose clauses are going  to be swapped.
 * @param old_index The index of one of the clauses to be swapped.
 * @param new_index The index of the other clause that is going to be swapped.
 *
 */

void swap_clauses( clause* formula, int old_index, int new_index ) {
    clause temp_clause;
    temp_clause = formula[old_index];
    formula[old_index] = formula[new_index];
    formula[new_index] = temp_clause;
}

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
                     int num_pure_literals ) {
    int i;
    for( i=0; i< num_pure_literals; i++ ) {

        if ( literal == pure_literals[i] ) {
            return TRUE;
        }
    }
    return FALSE;
}
