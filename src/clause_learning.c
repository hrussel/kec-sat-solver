/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Source code for the clause learning procedures of kecosats solver.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/


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
void add_cl_to_watcher_list( int clause_index ) {
    
    clause* cl = sat_st.formula + clause_index;
    
    variable *tail_watcher = cl->tail_watcher;
    int abs_tail_watcher   = abs(*tail_watcher);
    
    variable *head_watcher = cl->head_watcher;
    int abs_head_watcher   = abs(*head_watcher);
    
    list *watched_list;
    
    // Set the correct polarity to the watched head_literal
    if( *head_watcher > 0 ){
        watched_list = &sat_st.pos_watched_list[abs_head_watcher];
    }
    else {
        watched_list = &sat_st.neg_watched_list[abs_head_watcher];
    }
    
    // Add the clause cl to its head_watcher's watched list.
    push( watched_list, cl );
    
    // Set the correct polarity to the watched tail_literal
    if( *tail_watcher > 0 ){
        watched_list = &sat_st.pos_watched_list[abs_tail_watcher];
    }
    else {
        watched_list = &sat_st.neg_watched_list[abs_tail_watcher];
    }
    
    // Add the clause cl to its tail_watcher's watched list.
    push( watched_list, cl );

}


/**
 *
 *  This function saves the new learned clause of size 1 in a global stack.
 *  When certain number of clauses of size 1 have been learned, ocurrs an
 *  restart and the decided and propagated variables are erased.
 *  
 *  The restart algorithm will just let the preprocessing variables and the
 *  variables that ocurrs in the unit clauses with its own value. Then
 *  propagates this new learned clauses.
 *  
 *  @param lit The clause of size 1.
 **/
void learn_unit_clause( int* lit ){
    
    sat_gs.unit_learned_clauses++;
    push(&sat_st.unit_learned_clauses, (void*)lit[0]);
}

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
 * @pre lit is a unit clause.
 */

clause* learn_clause( int clause_length, int lit[] ){
    
    // Analyze the case of an empty clause that doesn't need to be learned
    if ( clause_length == 0 ){
        return NULL;
    }
    
    if ( clause_length > 15 ){
        free(lit);
        return NULL;
    }
    
    //TODO at this point we should decide if a learned clause needs to
    //     be deleted and replaced by the lit clause.
    if ( sat_st.clause_available_space == 0 ) {
        
        free(lit);
        return NULL;
    }
    
    // There is space to add the new learned clause
    if ( sat_st.clause_available_space > 0 ){
        
        //@assert The sat_st.formula[sat_st.num_clauses] is free.
        
        if ( clause_length > 1 ){
            
            clause* cl = sat_st.formula + sat_st.num_clauses;
            
            cl->size = clause_length;
            cl->head_watcher = NULL;
            cl->tail_watcher = NULL;
            cl->literals = lit;
            
            /* Set the tail watcher to the first unknown variable. At this
               point, there isn't another free variable in the clause.
             */
            int i=0;
            while ( sat_st.model[ abs(lit[i]) ] != UNKNOWN ){
                i++;
            }
            cl->tail_watcher = lit + i;
            
            /* The head_watcher needs to reference to the decision variable
               decided in the more recent instant.
             */
            if ( i > 0 ){
                cl->head_watcher = lit;
            } else {
                cl->head_watcher = lit + 1;
            }
            
            //@assert The two watched literals satisty the invariant of the
            //        watchers.
            
            i=0;
            while ( i<clause_length ){
                
                int abs_lit = abs(lit[i]);
                
                if ( sat_st.model[ abs_lit ] != UNKNOWN 
                     && lit + i != cl->tail_watcher
                     && sat_st.impl_graph[ abs_lit ] .decision_level
                            > sat_st.impl_graph[
                                    abs(* (cl->head_watcher) )].decision_level
                    )
                {
                    cl->head_watcher = lit+i;
                }
                i++;
            }
            
            // @assert cl->tail_watcher is free.
            // @assert cl->head_watcher has the maximum decision level.
            
            // Add to the tail_watcher list and head_watcher list the new clause
            // that is observing them.
            add_cl_to_watcher_list(cl - sat_st.formula);
            
            // Reduce the space available to learn clauses.
            sat_st.num_clauses++;
            sat_st.clause_available_space--;
            
            return cl;
        } else {
            
            // Clauses with one literal
            learn_unit_clause(lit);
            
            // This clause will be propagated after at restart.
            return NULL;
        }
    }
    
    return NULL;
}

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
 *  This function deletes the occurrence of this clause at watched list of
 *  its head and tail watcher.
 *  
 *  @param clause_index Index of clause that will be unlearned.
 *
 */
void remove_cl_from_watcher_list( int clause_index ) {
    clause* cl = sat_st.formula + clause_index;

    variable tail_watcher = *(cl->tail_watcher);
    int abs_tail_watcher  = abs(tail_watcher);

    variable head_watcher = *(cl->head_watcher);
    int abs_head_watcher  = abs(head_watcher);


    clause *current_clause;
    list *watched_list;

    if( head_watcher > 0 ){
        watched_list = &sat_st.pos_watched_list[abs_head_watcher];
    }
    else {
        watched_list = &sat_st.neg_watched_list[abs_head_watcher];
    }

    while( top(watched_list) != cl ) {
        // The pop procedure takes place at the beginning
        // of the list.
        current_clause = top(watched_list);
        pop(watched_list);
        
        // The enqueueing procedure takes place at the
        // end of the list that implements the stack.
        queue(watched_list, current_clause);
    }
    // Remove cl from head_watcher's watched_list.
    pop(watched_list);
    
    if( tail_watcher > 0 ){
        watched_list = &sat_st.pos_watched_list[abs_tail_watcher];
    }
    else {
        watched_list = &sat_st.neg_watched_list[abs_tail_watcher];
    }

    while( top(watched_list) != cl ) {
        // The pop procedure takes place at the beginning
        // of the list.
        current_clause = top(watched_list);
        pop(watched_list);
        
        // The enqueueing procedure takes place at the
        // end of the list that implements the stack.
        queue(watched_list, current_clause);
    }
    // Remove cl from tail_watcher's watched_list.
    pop(watched_list);

}
