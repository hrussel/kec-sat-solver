/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Source code for the KECOSATS main operations and procedures.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/
#include "kecosats_algorithm.h"
#include "conflict_analysis.h"

void free_decision_level_data(decision_level_data* dld){
    while(!empty(&dld->propagated_var)){
        pop(&dld->propagated_var);
    }
    
    free(dld);
}

int decide_next_branch(){
    //Create the structure that will be pushed
    decision_level_data *dec_lev_dat =
        (decision_level_data*)malloc(sizeof(decision_level_data));
    
    //Initialize the lists
    dec_lev_dat->propagated_var = *(new_list()); 

    //Choose the next unassigned variable. TODO do this more efficient!

    
    /*
    variable free_variable=1;
    while(  free_variable <= sat_st.num_vars &&
            sat_st.model[free_variable] != UNKNOWN
        ){
            free_variable++;
    }
    
    */
    variable free_variable=sat_st.num_vars + 1;
    int num_affected_cl = -1;
    variable free_v = 1;
    while(  free_v <= sat_st.num_vars ){
        
            if ( sat_st.model[free_v] == UNKNOWN
                && 
                ( sat_st.pos_watched_list[free_v].size
                    + sat_st.neg_watched_list[free_v].size > num_affected_cl)
                )
            {
                free_variable = free_v;
                num_affected_cl = sat_st.pos_watched_list[free_v].size
                                    + sat_st.neg_watched_list[free_v].size;
            }
            free_v++;
    }
    
    
    //If there are no more variables to assign, report an error
    if( free_variable > sat_st.num_vars ){
        push((&sat_st.backtracking_status), (void*)NULL);
        
        return SATISFIED;
    }
    
    sat_st.impl_graph[free_variable].decision_level
            = sat_st.backtracking_status.size + 1;
    sat_st.impl_graph[free_variable].conflictive_clause = NULL;
    
    if ( sat_st.pos_watched_list[free_variable].size
           > sat_st.neg_watched_list[free_variable].size )
    {
        free_variable = -free_variable;
    }
    
    /*
    if ( free_variable > 0 ){
        sat_st.model[free_variable] = TRUE;
    } else {
        sat_st.model[-free_variable] = FALSE;
    }
    */
    
    dec_lev_dat->assigned_literal = free_variable;
    dec_lev_dat->missing_branch = TRUE;
    
    //Push the structure in the stack
    push((&sat_st.backtracking_status), dec_lev_dat);
    
    return UNSATISFIED;
}

int preprocess(){
    
    int clause;
    
    stack unit_clauses;
    initialize_list(&unit_clauses);
    

    if( sat_gs.detect_pure_literals ){
        int num_pure_literals;
        int status;
        variable* pure_literals = find_pure_literals( &num_pure_literals );
        status = eliminate_pure_literals( pure_literals, num_pure_literals );
    }

    for (clause = 0;
         clause < sat_st.num_clauses;
         clause++)
    {
        if ( sat_st.formula[clause].head_watcher 
                == sat_st.formula[clause].tail_watcher)
        {
            push(&unit_clauses, &sat_st.formula[clause]);
        }
        
    }
    
    if ( unit_clauses.size > 0 ){
        return unit_propagation( &unit_clauses );
    } else {
        return DONT_CARE;
    }
}

int solve_sat(){
    
    int status = preprocess();
    
    if ( status == CONFLICT ){
        return UNSATISFIABLE;
    } else if ( status == SATISFIED ){
        return SATISFIABLE;
    }
    
    //The algorithm is an iterative backtracking.

    //Decide next branch, and push it as a
    //decision_level_data structure in the
    //backtracking_status variable of the global
    //sat_st variable.
    if ( decide_next_branch() == SATISFIED ){
        return SATISFIABLE;
    }
    
    while ( TRUE ){
        
        //Check the top variable in the stack, if the
        //stack is empty, then all possible assignments were tried
        //and the formula is UNSATISFIABLE
        if( empty(&sat_st.backtracking_status) )
            return UNSATISFIABLE;
        
        decision_level_data *top_el =
            (decision_level_data*) top(&sat_st.backtracking_status);
        
        //Make the assignment of the literal that appeared on top
        //of the stack
        
        printf("Decido %d\n", top_el->assigned_literal);
        
        int assignment_result = deduce(top_el->assigned_literal);
        
        //If the result is UNKNOWN, continue the recursion (iteratively)
        if( assignment_result == DONT_CARE ){
            
            if (decide_next_branch() == SATISFIED){
                return SATISFIABLE;
            }
            continue;
        }
        
        //If the assignment satisfied the formula, return a positive
        //answer and finish the funtion
        else if( assignment_result == UNIT_CLAUSE ){  //TODO this should be SATISFIED
            
            // OJOOOOOOOO: TAL VEZ SE ELIMINE ESTO
            //             REVISAR SI DEDUCE PUEDE RETORNAR UNIT_CLAUSE
            //             SE CREE QUE NO
            
            return SATISFIABLE;
        }
        
        //If the assignment made the formula FALSE, then backtrack until
        //a variable that can be flipped is found.
        else if( assignment_result == CONFLICT ){
            
            /*
            {
                int i;
                for (i = 1; i<= sat_st.num_vars; i++){
                    if ( sat_st.impl_graph[i].decision_level != -1 ){
                        printf("(%d,%d,%d)\n",i, sat_st.impl_graph[i].decision_level,
                                                sat_st.impl_graph[i].conflictive_clause - sat_st.formula);
                    }
                }
                printf("\n");
            }
            */
            
            int* lit;
            int clause_length;
            
            int backtrack_to_level;
            backtrack_to_level = analyze_conflict(&lit, &clause_length);
            backtrack_to_level = sat_st.backtracking_status.size;
            
            // Return to the target level, which can be a non-chronological
            // jump.
            
            while ( sat_st.backtracking_status.size > backtrack_to_level ){
                
                printf("no cronologico\n");
                while(1);
                undo_assignments( top(&sat_st.backtracking_status) );
                pop( & sat_st.backtracking_status );
            }
            
            //@assert At this point we are at the maximum decision_level
            //        corresponding to the conflict found.
            
            //If the stack got empty, then no backtracking was
            //possible, hence, the formula is UNSATISFIABLE
            if( empty(&sat_st.backtracking_status) )
                return UNSATISFIABLE;
            
            //printf("Conflicto %d -> %d\n", sat_st.backtracking_status.size, backtrack_to_level);
            
            clause* learned_clause = NULL; //learn_clause(clause_length, lit);
            
            // Jump to the maximum decision_level with a non-flipped variable.
            // Destroy the structure and continue the search for a variable that
            // can be flipped.
            
            while ( !empty(&sat_st.backtracking_status) 
                            && ((decision_level_data*)
                                    top(&sat_st.backtracking_status)
                                )->missing_branch == FALSE )
            {
                learned_clause = NULL;
                
                top_el = (decision_level_data*)top(&sat_st.backtracking_status);
                
                undo_assignments(top_el);
                free_decision_level_data(top_el);
                
                pop(&sat_st.backtracking_status);
            }
            
            //@assert At this point we are at the maximum decision_level
            //        corresponding to the conflict found and "flippable"
            //        literal.
            
            /*@assert If the assigned_literal corresponding to the
                      backtrack_to_level level in the stack has no
                      missing_branch (all possible assignments to this variable
                      have already been tested), then the next flip is a
                      decision assignment and not an implication assignment.
                      Hence, its Antecedent asssignment set is void (the
                      conflictive_clause doesn't affect the assigned_literal).
            */
            
            if( !empty( &sat_st.backtracking_status) ){
                
                top_el = (decision_level_data*)top(&sat_st.backtracking_status);
                
                undo_assignments(top_el);
                
                //If the opposite branch has not been tried,
                //then flip the assignment and continue the
                //recursion (iteratively)
                top_el->assigned_literal = -top_el->assigned_literal; 
                top_el->missing_branch = FALSE;
                
                sat_st.impl_graph[abs(top_el->
                                    assigned_literal)].decision_level
                    = sat_st.backtracking_status.size;
                
                sat_st.impl_graph[abs(top_el->assigned_literal)
                                    ].conflictive_clause = learned_clause;
            } else {            
                return UNSATISFIABLE;
            }
            
        }
    }
    
    return UNSATISFIABLE;
}

void undo_assignments(decision_level_data *dec_lev_dat){
    //First, unset the assigned variable in the
    //given decision level
    variable assigned_var = abs(dec_lev_dat->assigned_literal);
    
    sat_st.model[ assigned_var ] = UNKNOWN;
    sat_st.impl_graph[ assigned_var].decision_level = -1;
    sat_st.impl_graph[ assigned_var].conflictive_clause = NULL;
    
    //Similarly, unset the assignments for the propageted
    //literals
    while( !empty(&dec_lev_dat->propagated_var) ){
        
        variable *watcher =
            (variable*)top(&dec_lev_dat->propagated_var);
        
        assigned_var = abs(*watcher);
        sat_st.model[ assigned_var ] = UNKNOWN;
        
        sat_st.impl_graph[ assigned_var].decision_level = -1;
        sat_st.impl_graph[ assigned_var].conflictive_clause = NULL;
        
        pop(&dec_lev_dat->propagated_var);

    }

}

int deduce( variable literal ) {
    
    // A variable is a signed integer. If the sign is '-' the variable
    // abs(literal) was assigned a false value. If the sign is '+', it was
    // assigned a truth value.
    variable abs_literal = abs( literal );
    
    if ( sat_st.model[abs_literal] != UNKNOWN ){
        if ((sat_st.model[abs_literal] == TRUE) == ( literal > 0)){
            return DONT_CARE;
        } else {
            return CONFLICT;
        }
    }
    
    list* clauses_affected;
    
    sat_st.model[abs_literal] = (abs_literal/literal == 1 ? TRUE : FALSE);
    
    if ( sat_st.model[abs_literal] == TRUE ) {
        clauses_affected = &(sat_st.neg_watched_list[abs_literal]);
    } else {
        clauses_affected = &(sat_st.pos_watched_list[abs_literal]);
    }
    
    if ( clauses_affected->size > 0 ) {
        return set_newly_watchers( clauses_affected, literal );        
    }
    
    return DONT_CARE;
}

int set_newly_watchers( list* clauses_affected, variable literal )
{
    
    stack unit_clauses, clauses_affected_tmp;
    initialize_list(&unit_clauses);
    initialize_list(&clauses_affected_tmp);
    
    int status = DONT_CARE;
    
    while( !empty(clauses_affected) ){
        clause* cl = (clause*)top(clauses_affected);
        pop(clauses_affected);
        
        push(&clauses_affected_tmp, cl);
    }
    
    while( !empty(&clauses_affected_tmp) && status == DONT_CARE ) {
        
        clause* cl = (clause*)top(&clauses_affected_tmp);
        pop(&clauses_affected_tmp);
        
        if ( is_head_watcher(cl, literal) ) {
            status = update_watcher( cl );
        }
        else if ( is_tail_watcher(cl, literal) ) {
            swap_watchers( cl );
            status = update_watcher( cl );
        }
        
        // We maintain a stack of unitary clauses which need further
        // propagation. (Since they are unitary we need to properly assign their
        // single variables). We save them for a little later.
        if (status == UNIT_CLAUSE){
          // @Assert: cl->tail_watcher points to the single variable of the
          // unitary clause.
            push( &unit_clauses, cl );
            status = DONT_CARE;
        }
        
        add_to_watched_list(*cl->head_watcher, cl);
    }
    
    //@Assert: status== CONFLICT || status == DONT_CARE.
    
    if (status == DONT_CARE){
        return unit_propagation( &unit_clauses );
    } else {
        
        //@Assert: status == CONFLICT.
        
        while( !empty(&unit_clauses) ){
            pop( &unit_clauses );
        }
        
        while( !empty(&clauses_affected_tmp) ){
            clause *cl = (clause*)top(&clauses_affected_tmp);
            pop( &clauses_affected_tmp);
            add_to_watched_list(*cl->head_watcher, cl);
        }
        
        return status;
    }
}

int unit_propagation( stack* unit_clauses )
{
    
    int status = DONT_CARE;
    decision_level_data* dec_level_data
            = (decision_level_data*) top( &(sat_st.backtracking_status) );
    
    while( !empty(unit_clauses ) && status == DONT_CARE){
        
        clause* cl = (clause*)top(unit_clauses);
        pop(unit_clauses);
        
        // We need to keep track of the variables that were propagated, to
        // ensure the correctness of the backtracking procedure.
        if ( dec_level_data ){
            push( &(dec_level_data->propagated_var), cl->tail_watcher );
        }
        
        // Add this implication edge to the implication graph.
        {
            int implied_var = abs(*cl->tail_watcher);
            
            // OBS: P Q PUSIMOS + 1?
            
            sat_st.impl_graph[implied_var].decision_level
                = sat_st.backtracking_status.size;
            sat_st.impl_graph[implied_var].conflictive_clause = cl;
        }
        
        // Propagate the single variable in each the unitary clause. The
        // tail_watcher points to its single variable.
        printf("   De la decision: %d propago: %d por la clausula no. %d\n", dec_level_data->assigned_literal, *cl->tail_watcher, cl-sat_st.formula);
        printf("      el antecesor de %d con nivel de decision %d cuando la pila tiene tam %d \n", *cl->tail_watcher, sat_st.impl_graph[abs(*cl->tail_watcher)].decision_level, sat_st.backtracking_status.size);

        status = deduce( *cl->tail_watcher );
    }
    
    while( !empty(unit_clauses ) ){
        pop(unit_clauses);
    }
    
    return status;
}


