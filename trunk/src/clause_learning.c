#include "pure_literals.h"
#include "sat_io.h"
#include "clause_learning.h"

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

void learn_clause( int clause_length, int lit[] ){
    
    int erased = FALSE;
    int clause_index = sat_st.num_clauses;
    
    if ( sat_st.clause_available_space == 0 ) {
        /* Decidir cual clausula se borra */
        clause_index = choose_victim_clause();
        
        if ( sat_st.formula[clause_index].size >= clause_length ){
            erased = TRUE;
            unlearn_clause(clause_index);
        }
    }
    
    if ( sat_st.clause_available_space > 0 ){
        /* Aprender clausula */
        sat_st.clause_available_space--;
        
        // Set watchers lists and initialize clause.
        set_clause( &sat_st.formula[clause_index], clause_length, lit );
        
        sat_st.num_clauses++;
    }
}

/* OJOOOOOOOOOO: CAMBIAR LOS WATCHERS DE LAS DOS CLAUSULAS
 */

/**
 *
 * Deletes the clause indexed by @e clause_index from the formula.
 * @param clause_index The index of the clause to be deleted.
 *
 *
 */
void unlearn_clause( int clause_index ) {
    int last_clause_index = sat_st.num_clauses-1;
    swap_clauses( sat_st.formula, clause_index, last_clause_index );
    // remove_cl_from_watcher_list( clause_index );
    // remove_cl_from_watcher_list( last_clause_index );
    // add_cl_to_watcher_list( last_clause_index );
    free( sat_st.formula[last_clause_index].literals );
    
    sat_st.num_clauses--;
    sat_st.clause_available_space++;
}

/**
 *
 * This function searches for all clauses tagged @e too_large: those
 * which have more than @esat_st.clause_upper_bound literals. If one of these
 * clauses is found to be non-unitary, it will be unlearned (discarded). Otherwise,
 * it will be preserved.
 *
 */
void unlearn_large_clauses() {
    int status;
    int current_clause;
    for (current_clause = sat_st.num_original_clauses;
         current_clause < sat_st.num_clauses;
         current_clause++)
    {
        if ( sat_st.formula[current_clause].too_large == TRUE ) {
            // A learned clause marked for possible deletion
            // has been found. If it's not a unitary clause
            // it may be deleted. Otherwise, it will be kept.
                
            status = update_watcher( &sat_st.formula[current_clause] );

            if ( status == UNIT_CLAUSE ) {
                unlearn_clause( current_clause );
            }
        }
    }
}

void remove_cl_from_watcher_list( int clause_index ) {
    clause* cl = sat_st.formula + clause_index;

    variable tail_watcher = *(cl->tail_watcher);
    int abs_tail_watcher  = abs(tail_watcher);

    variable head_watcher = *(cl->head_watcher);
    int abs_head_watcher  = abs(head_watcher);


    clause *current_clause;
    list *watched_list;

    if( head_watcher > 0 ){
        watched_list = pos_watched_list[abs_head_watcher];
    }
    else {
        watched_list = neg_watched_list[abs_head_watcher];
    }

    while( top(watched_list) != cl ) {
        // The pop procedure takes place at the beginning
        // of the list.
        current_clause = pop(watched_list);
        // The enqueueing procedure takes place at the
        // end of the list that implements the stack.
        queue(watched_list, current_clause);
    }
    // Remove cl from head_watcher's watched_list.
    pop(watched_list);

    // tail_watcher
    // OJO Codigo repetido.....

    if( tail_watcher > 0 ){
        watched_list = pos_watched_list[abs_tail_watcher];
    }
    else {
        watched_list = neg_watched_list[abs_tail_watcher];
    }

    while( top(watched_list) != cl ) {
        // The pop procedure takes place at the beginning
        // of the list.
        current_clause = pop(watched_list);
        // The enqueueing procedure takes place at the
        // end of the list that implements the stack.
        queue(watched_list, current_clause);
    }
    // Remove cl from tail_watcher's watched_list.
    pop(watched_list);

}

void add_cl_to_watcher_list( int clause_index ) {
    clause* cl = sat_st.formula + clause_index;

    variable *tail_watcher = cl->tail_watcher;
    int abs_tail_watcher   = abs(*tail_watcher);

    variable *head_watcher = cl->head_watcher;
    int abs_head_watcher   = abs(*head_watcher);

    list *watched_list;

    if( head_watcher > 0 ){
        watched_list = pos_watched_list[abs_head_watcher];
    }
    else {
        watched_list = neg_watched_list[abs_head_watcher];
    }

    // Add the clause cl to its head_watcher's watched list.
    push( watched_list, cl );

    if( tail_watcher > 0 ){
        watched_list = pos_watched_list[abs_tail_watcher];
    }
    else {
        watched_list = neg_watched_list[abs_tail_watcher];
    }

    // Add the clause cl to its tail_watcher's watched list.
    push( watched_list, cl );

}
