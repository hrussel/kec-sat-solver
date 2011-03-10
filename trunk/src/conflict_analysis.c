/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Source code for the conflict anaylisis procedures.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/

#include "conflict_analysis.h"

int* visited;   // A array of booleans that will be used in the traversing
                // of the implication graph.

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
int get_bt_target_level( int clause_length, int *conflict_clause ) {
    
    int i, current_max=-1;
    decision_node* current_dec_node;
    
    for( i=0; i<clause_length; i++ ) {
        current_dec_node = sat_st.impl_graph + abs(conflict_clause[i]);
        current_max = max(current_max, current_dec_node->decision_level);
    }
    
    return current_max;
}

extern clause* orig_conflict_clause; // This clause is the responsible of the
                                     // conflict.

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
int* get_conflict_induced_cl( variable abs_literal, int* conflict_cl_length ) {
    
    clause* unit_conflict_clause = orig_conflict_clause;
    
    // If there aren't conflictive clauses, then there isn't clause to learn.
    if ( unit_conflict_clause == NULL ){
        *conflict_cl_length = 0;
        return NULL;
    }
    
    // Set each variable unvisited.
    memset(visited, 0, (sat_st.num_vars+1)*sizeof(int));
    
    stack reachable;                // Stack of reached variables.
    initialize_list(&reachable);
    
    stack conflict_induced_cl;      // Stack of decision literals that are in
                                    // the learned clause.
    
    initialize_list(&conflict_induced_cl);
    
    // Set directed predecessors reachables
    {
        int i, cl_size;
        
        cl_size = unit_conflict_clause->size;
        
        // All literals that are in the conflictive clause are reached by the
        // conflict.
        for( i=0; i<cl_size; i++ ) {
            push(&reachable, &unit_conflict_clause->literals[i]);
        }
    }
    
    // Visit each variable reached until fixed point.
    while(!empty(&reachable)){
        
        int pred = *((int*)top(&reachable));
        int abs_pred = abs(pred);
        
        // Each variable will be expanded only once.
        if ( !visited[abs_pred] ){
            
            // Set the variable as visited.
            visited[abs_pred] = TRUE;
            
            if ( sat_st.impl_graph[abs_pred].conflictive_clause == NULL){
                
                // @assert abs_pred is a decision variable, then it will be
                //         in the learned clause.
                
                push(&conflict_induced_cl, top(&reachable));
                pop(&reachable);
                
            } else {
                
                pop(&reachable);
                
                clause *cl = sat_st.impl_graph[abs_pred].conflictive_clause;
                
                int cl_size = cl->size;
                int i;
                
                // Set each predecessor to the current clause as reachable.
                for( i=0; i<cl_size; i++ ) {
                    int pred2 = abs(cl->literals[i]);
                    
                    if ( pred2 != abs_pred && !visited[pred2] )
                    {
                        push(&reachable, &cl->literals[i]);
                    }
                }
            }
            
        } else {
            pop(&reachable);
        }
    }
    
    int i = 0;
    int* clause = (int*)malloc(conflict_induced_cl.size*sizeof(int));
    
    // Construct the conflictive clause
    while( !empty(&conflict_induced_cl) ){
        
        clause[i] = abs( *((int*)top(&conflict_induced_cl)) );
        
        if ( sat_st.model[ clause[i] ] > 0 ){
            clause[i] = -abs(clause[i]);
        } 
        else if ( sat_st.model[clause[i]] < 0 ) {
            clause[i] = abs(clause[i]);
        }
        
        pop(&conflict_induced_cl);
        i++;
    }
    
    *conflict_cl_length = i;
    
    // Remove the conflict
    orig_conflict_clause = NULL;
    
    return clause;
}

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
int analyze_conflict(int** conflictive_clause, int* clause_length) {
    
    // Will hold the length of the conflict_induced_clause.
    
    int conflictive_variable;
    
    decision_level_data* dld = top(&sat_st.backtracking_status);
    
    if ( empty(&dld->propagated_var) ){
        conflictive_variable = abs(dld->assigned_literal);
    } else {
        conflictive_variable = abs(*(int*)top(&dld->propagated_var));
    }
    
    *conflictive_clause = 
                get_conflict_induced_cl( conflictive_variable, clause_length );
    
    if ( *clause_length > 0 ){
        int target_level = get_bt_target_level(*clause_length,
                                               *conflictive_clause);
        
        return target_level;
    }
    
    return sat_st.backtracking_status.size;
}
