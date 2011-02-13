#include "sat.h"

void free_decision_level_data(decision_level_data* dld){
    //free_list(dld->propagated_var);
    //free_list(dld->satisfied_clauses);
    free(dld);
}

/**
 * The purpose of this procedure is to add the clause cl to the list of
 * clauses where v is watched positively or negatively.
 *
 * @param v The variable that is watched.
 * @param cl A pointer to the clause where v is watched
 *
 */
inline void add_to_watched_list(variable v, clause* cl){
    
    if ( v > 0 ){
        push(&sat_st.pos_watched_list[abs(v)], cl);
    } else {
        push(&sat_st.neg_watched_list[abs(v)], cl);
    }
}

/**
  Choose a next variable, push the info in the backtracking_status
  stack. MAke sure that if the stack is empty is because it's
  the first assignment (else you'll get an infinite loop in the solver).

  This next branch things takes care of flippings! And also
  backtracking-stuff due to this flippings!

  Return TRUE or FALSE if there where more variables to assign
 */
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
    
    if ( sat_st.pos_watched_list[free_variable].size
           > sat_st.neg_watched_list[free_variable].size )
    {
        free_variable = -free_variable;
    }
    
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
    
    int num_pure_literals;
    int status;
    // variable* pure_literals = find_pure_literals( &num_pure_literals );
    // status = eliminate_pure_literals( pure_literals, num_pure_literals );

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

/**
 * This function tries to solve the sat_instance that is stored
 * in the global variable sat_st.
 *
 * @return   SATISFIABLE if an assignment to the SAT variables is
 *           found that satisfies the formula. Otherwise, UNSATISFIABLE.
 */
int solve_sat(){
    
    int status = preprocess();

    if ( status != DONT_CARE ){
        return UNSATISFIABLE;
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
        
        //printf("solve_sat: deducing -> %d\n",top_el->assigned_literal);
        int assignment_result = deduce(top_el->assigned_literal);
        //printf("solve_sat: result of %d -> ",top_el->assigned_literal);
        
        //If the result is UNKNOWN, continue the recursion (iteratively)
        if( assignment_result == DONT_CARE ){
            //printf("dont care\n");
            
            if (decide_next_branch() == SATISFIED){
                return SATISFIABLE;
            }
            continue;
        }
        
        //If the assignment satisfied the formula, return a positive
        //answer and finish the funtion
        else if( assignment_result == UNIT_CLAUSE ){  //TODO this should be SATISFIED
            //printf("SATISFIED!\n");
            return SATISFIABLE;
        }
        //If the assignment made the formula FALSE, then backtrack until
        //a variable that can be flipped is found.
        else if( assignment_result == CONFLICT ){ 
            
            //printf("CONFLICT!\n");

            decision_level_data *top_el = NULL;

            do {

                top_el = (decision_level_data*) 
                    top(&sat_st.backtracking_status);

                //Undo the assignments made in the
                //decision level
                undo_assignments(top_el);

                if( top_el->missing_branch == TRUE ){
                    //If the opposite branch has not been tried,
                    //then flip the assignment and continue the
                    //recursion (iteratively)

                    //Flip the value
                    top_el->assigned_literal =
                        - top_el->assigned_literal; 

                    top_el->missing_branch = FALSE;
                    break;

                } else {
                    //Destroy the structure and continue
                    //the search for a variable that can be flipped
                    free_decision_level_data(top_el);
                    pop(&sat_st.backtracking_status);
                }

            } while( !empty(&sat_st.backtracking_status) );
            
            //If the stack got empty, then no backtracking was
            //possible, hence, the formula is UNSATISFIABLE
            if( empty(&sat_st.backtracking_status) )
                return UNSATISFIABLE;
        }

    }

    return UNSATISFIABLE;
}

void undo_assignments(decision_level_data *dec_lev_dat){
    //First, unset the assigned variable in the
    //given decision level
    variable assigned_var = abs(dec_lev_dat->assigned_literal);

    sat_st.model[ assigned_var ] = UNKNOWN;
    //printf("Undoing variable: %d\n",assigned_var);

    //Similarly, unset the assignments for the propageted
    //literals
    while( !empty(&dec_lev_dat->propagated_var) ){

        variable *watcher =
            (variable*)top(&dec_lev_dat->propagated_var);

        assigned_var = abs(*watcher);
        sat_st.model[ assigned_var ] = UNKNOWN;
        //printf("Undoing variable: %d\n",assigned_var);
        
        pop(&dec_lev_dat->propagated_var);

    }

}

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

int unit_propagation( stack* unit_clauses )
{
    
    int status = DONT_CARE;
    decision_level_data* dec_level_data
            = (decision_level_data*) top( &(sat_st.backtracking_status) );
    
    while( !empty(unit_clauses ) && status == DONT_CARE){
        
        clause* cl = (clause*)top(unit_clauses);
        // We need to keep track of the variables that were propagated, to
        // ensure the correctness of the backtracking procedure.
        if ( dec_level_data ){
            queue( &(dec_level_data->propagated_var), cl->tail_watcher );
        }
        
        // Propagate the single variable in each the unitary clause. The
        // tail_watcher points to its single variable.
        status = deduce( *cl->tail_watcher );
        
        pop(unit_clauses);
    }
    
    while( !empty(unit_clauses ) ){
        pop(unit_clauses);
    }
    
    return status;
}

/**
 * Returns the value in the model for a literal. If this literal hasn't still
 * been assigned it returns UNKNOWN(-1).
 * @param literal A pointer to the variable about which we would like to know
 *        its value.
 * @return TRUE(1)     if the value has been assigned TRUE.
 *         FALSE(0)    if the value has been assigned FALSE.
 *         UNKNOWN(-1) if the value has not been assigned.
 */
int current_literal_value( variable* literal ) {
  return sat_st.model[abs(*literal)];
}

/**
 * 
 * @param cl A pointer to a clause.
 * @param literal A variable.
 * @return TRUE(1)     if the variable @e literal is the head watcher of @e cl.
 *         FALSE(0)    if the variable @e literal isnt' the head watcher of @e
 *                     cl. 
 */

int is_head_watcher( clause* cl, variable literal ){
    return abs(*(cl->head_watcher)) == abs(literal);
}

/**
 * 
 * @param cl A pointer to a clause.
 * @param literal A variable.
 * @return TRUE(1)     if the variable @e literal is the tail watcher of @e cl.
 *         FALSE(0)    if the variable @e literal isnt' the tail watcher of @e 
 *                     cl.
 */

int is_tail_watcher( clause* cl, variable literal ){
    return abs(*(cl->tail_watcher)) == abs(literal);
}

/**
 * This functions swaps the head_watcher and the tail_watcher of the clause cl.
 *
 * @param cl A clause parameter
 * @pre cl != NULL;
 *
 */
void swap_watchers( clause* cl ){
    variable* tmp = cl->head_watcher;
    cl->head_watcher = cl->tail_watcher;
    cl->tail_watcher = tmp;
}

/**
 * Return true iff the literal v is true with the current assignment.
 *
 * @param v
 * @return UNIT_CLAUSE(1) If the clause head_clause is unitary.
 *         CONFLICT(2)    If the clause is conflictive with the current model.
 *         DONT_CARE(0)   Neither of the previous two alternatives.
 * @pre 1 <= abs(v) <= sat_st.num_vars
 *
 */
int is_satisfied( variable v ){
    
    int abs_v = abs(v);
    
    if ( sat_st.model[ abs_v ] == UNKNOWN ){
        return FALSE;
    }
    
    if ( (sat_st.model[ abs_v ] == FALSE && v < 0)
            || (sat_st.model[ abs_v ] == TRUE && v > 0)
        )
    {
        return TRUE;
    }
    
    return FALSE;
}

/**
 * Given a clause and a freshly recently assigned variable occurring in the
 * aforementioned clause that is being pointed to by the head_watcher, searches
 * for some other valid literal to be the new head_watcher. 
 * This method determines if a clause a unitary clause or a conflictive clause.
 *
 * @param head_clause
 * @return UNIT_CLAUSE(1) If the clause head_clause is unitary.
 *         CONFLICT(2)    If the clause is conflictive with the current model.
 *         DONT_CARE(0)   Neither of the previous two alternatives.
 * @pre head_clause != NULL
 *
 */

int update_watcher( clause* clause ) {
    
    variable current_literal;
    
    // This variable will indicate if in the clause @head_clause there's an
    // unassigned literal.
    int exists_free_literal = FALSE;  
    
    {
        int i;
        // Iterate over the clause head_clause searching for an unassigned
        // literal not pointed to by the tail_watcher. If it finds one such
        // unassigned literal, make the head_watcher point to it.
        for ( i=0; i < clause->size; i++ ){
            current_literal = abs( clause->literals[i] );
            
            if ( (sat_st.model[current_literal] == UNKNOWN
                    || is_satisfied(clause->literals[i]))
                && clause->tail_watcher != clause->literals + i )
            {
                // @Assert: There's an unassigned literal not pointed to by the
                // tail_watcher.
                clause->head_watcher = (clause->literals) + i;
                exists_free_literal = TRUE;
                break;
            }
        }
    }
    
    if ( !exists_free_literal ) {
        
        // @Assert: Either all literals are assigned or the only unassigned
        // literal is pointed to by the tail_watcher.
        
        // assign( *(head_clause->tail_watcher), TRUE );
        
        if ( current_literal_value(clause->tail_watcher) == UNKNOWN ) {
          // @Assert: This clause is a unit clause.
            return UNIT_CLAUSE;
        } else if ( is_satisfied(*(clause->tail_watcher)) ){
            return DONT_CARE;
        }
        
        // @Assert: This clause is not satisfiable. The literal pointed to by
        // the tail_watcher is already assigned and evaluates to False. 
        // We require here that head_clause->satisfied != TRUE.
        return CONFLICT;
    }
    // @ Assert: We found an unassigned literal distinct not pointed to by the
    // tail_watcher. 
    return DONT_CARE;
}